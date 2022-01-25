// Fill out your copyright notice in the Description page of Project Settings.


#include "RenameUtilityWidget.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Components/Button.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Components/DetailsView.h"
#include "Components/SinglePropertyView.h"
#include "Misc/DateTime.h"


void URenameUtilityWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_RenameAssets)
	{
		Button_RenameAssets->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::BatchRename);
	}

	if (PropertyView_NewName)
	{
		PropertyView_NewName->SetObject(this);
		PropertyView_NewName->SetPropertyName("NewName");
	}

	if (Button_AddPrefix)
	{
		Button_AddPrefix->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::CheckAndAddPrefix);
	}

	if (DetailsView_PrefixProperties)
	{
		DetailsView_PrefixProperties->SetObject(this);
		DetailsView_PrefixProperties->CategoriesToShow.AddUnique("AddPrefix");
		DetailsView_PrefixProperties->PropertiesToShow.AddUnique("PrefixesMap");
		DetailsView_PrefixProperties->PropertiesToShow.AddUnique("bCheckAllAssets");
		DetailsView_PrefixProperties->bAllowFiltering = false;
	}
}

void URenameUtilityWidget::BatchRename()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	if (SelectedAssets.Num() == 0)
	{
		// TODO Print error
		return;
	}

	if (NewName == "" || NewName.Contains(" "))
	{
		// TODO Print error
		return;
	}

	uint32 Counter = 0;

	FScopedSlowTask RenameSelectedAssets(SelectedAssets.Num(),
	                                     FText::FromString(RenameMessage));
	RenameSelectedAssets.MakeDialog();

	UE_LOG(LogRenameUtility, Display, TEXT("Create batch rename log file."));
	FString FileName = "BatchRename";
	GenerateFileName(FileName);
	SaveToLogFile("Start batch renaming\n", FileName);

	for (UObject* Asset : SelectedAssets)
	{
		const FString UpdatedMessage = RenameMessage + FString::Printf(
			TEXT(" %d/%d"),
			SelectedAssets.IndexOfByKey(Asset),
			SelectedAssets.Num());
		RenameSelectedAssets.EnterProgressFrame(1, FText::FromString(UpdatedMessage));

		if (!ensure(Asset))
		{
			SaveToLogFile(FString::Printf(TEXT("Can't rename %s"), *Asset->GetName()), FileName);
			continue;
		}

		const FString OldName = Asset->GetName();
		Counter++;
		FString FinalName = NewName + FString::Printf(TEXT("_%d"), Counter);

		if (!PrefixesMapIsEmpty())
		{
			const UClass* AssetClass = Asset->GetClass();

			if (!PrefixesMap.Find(AssetClass))
			{
				SaveToLogFile(FString::Printf(TEXT("Can't find prefix for class %s"), *AssetClass->GetName()),
				              FileName);
			}
			else
			{
				FString Prefix = PrefixesMap[AssetClass];
				GeneratePrefix(Prefix);
				const FString Name = Asset->GetName();

				if (!FinalName.StartsWith(Prefix, ESearchCase::CaseSensitive))
				{
					FinalName = Prefix + FinalName;
				}
			}
		}

		UEditorUtilityLibrary::RenameAsset(Asset, FinalName);

		FString LogMessage = FString::Printf(
			TEXT("Rename %s to %s in %s"),
			*OldName,
			*FinalName,
			*Asset->GetPathName());

		UE_LOG(LogRenameUtility, Warning, TEXT("%s"), *LogMessage);
		SaveToLogFile(LogMessage, FileName);
	}

	SaveToLogFile("", FileName);
	SaveToLogFile("Finish batch renaming", FileName);
	NewName = "";
}

void URenameUtilityWidget::CheckAndAddPrefix()
{
	if (PrefixesMapIsEmpty()) return;

	if (bCheckAllAssets)
	{
		// TODO Add check all assets in the project
		return;
	}

	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	UE_LOG(LogRenameUtility, Display, TEXT("Create add prefix log file."));
	FScopedSlowTask AddPrefixProgress(SelectedAssets.Num(),
	                                  FText::FromString(AddPrefixMessage));
	AddPrefixProgress.MakeDialog();

	FString FileName = "AddPrefix";
	GenerateFileName(FileName);
	SaveToLogFile("Start adding prefixes\n", FileName);

	for (UObject* Asset : SelectedAssets)
	{
		const FString UpdatedMessage = AddPrefixMessage + FString::Printf(
			TEXT(" %d/%d"),
			SelectedAssets.IndexOfByKey(Asset),
			SelectedAssets.Num());
		AddPrefixProgress.EnterProgressFrame(1, FText::FromString(UpdatedMessage));

		if (!ensure(Asset))
		{
			// TODO Log error
			continue;
		}

		const UClass* AssetClass = Asset->GetClass();

		if (!PrefixesMap.Find(AssetClass))
		{
			SaveToLogFile(FString::Printf(TEXT("Can't find prefix for class %s"), *AssetClass->GetName()), FileName);
			continue;
		}

		FString Prefix = PrefixesMap[AssetClass];
		GeneratePrefix(Prefix);
		
		const FString Name = Asset->GetName();

		if (!Name.StartsWith(Prefix, ESearchCase::CaseSensitive))
		{
			UEditorUtilityLibrary::RenameAsset(Asset, Prefix + Name);
			SaveToLogFile(
				FString::Printf(
					TEXT("Add prefix %s to %s, new name is %s. %s"),
					*Prefix,
					*Name,
					*Asset->GetName(),
					*Asset->GetPathName()),
				FileName);
		}
	}

	SaveToLogFile("", FileName);
	SaveToLogFile("Finish adding prefixes", FileName);
}

bool URenameUtilityWidget::PrefixesMapIsEmpty() const
{
	if (PrefixesMap.Num() == 0)
	{
		UE_LOG(LogRenameUtility, Error, TEXT("Prefixes map is empty, can't add prefixes."));
	}

	return PrefixesMap.Num() == 0;
}

void URenameUtilityWidget::GeneratePrefix(FString& Prefix)
{
	if (Prefix.Contains("_")) return;

	Prefix += "_";
}

void URenameUtilityWidget::GenerateFileName(FString& Name)
{
	FString Date = "";
	GetDate(Date);
	Name.Append("_" + Date);
}

void URenameUtilityWidget::SaveToLogFile(const FString& Message, const FString& FileName)
{
	FString Date;
	GetDate(Date);
	const FString LogMessage = Message == ""
		                           ? FString("\n")
		                           : FString::Printf(TEXT("%s | %s\n"), *Date, *Message);

	// TODO Add message type SUCCESS/ERROR/WARNING
	const FString LogFileName = "Log_" + FileName + ".txt";
	const FString FilePath = FPaths::ProjectDir() + "Saved/Logs/TrickyAutomation/" + LogFileName;
	FFileHelper::SaveStringToFile(LogMessage,
	                              *FilePath,
	                              FFileHelper::EEncodingOptions::AutoDetect,
	                              &IFileManager::Get(),
	                              FILEWRITE_Append);
}

void URenameUtilityWidget::GetDate(FString& Date)
{
	const FDateTime CurrentTime = FDateTime::Now();
	Date = FString::Printf(TEXT("%d%02d%02d_%02d%02d%02d"),
	                       CurrentTime.GetYear(),
	                       CurrentTime.GetMonth(),
	                       CurrentTime.GetDay(),
	                       CurrentTime.GetHour(),
	                       CurrentTime.GetMinute(),
	                       CurrentTime.GetSecond());
}
