// Fill out your copyright notice in the Description page of Project Settings.


#include "RenameUtilityWidget.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Components/DetailsView.h"
#include "Components/SinglePropertyView.h"
#include "Misc/DateTime.h"


void URenameUtilityWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	auto FillCombobox = [&](UComboBoxString* ComboBox)
	{
		ComboBox->AddOption("NONE");

		if (SuffixesArray.Num() > 0)
		{
			ComboBox->ClearOptions();
			ComboBox->AddOption("NONE");

			for (int32 i = 0; i < SuffixesArray.Num(); i++)
			{
				ComboBox->AddOption(SuffixesArray[i]);
			}
		}

		ComboBox->SetSelectedIndex(0);
	};

	if (Button_RenameAssets)
	{
		Button_RenameAssets->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::BatchRename);
	}

	if (PropertyView_NewName)
	{
		PropertyView_NewName->SetObject(this);
		PropertyView_NewName->SetPropertyName("NewName");
	}

	if (ComboBox_SuffixRename)
	{
		FillCombobox(ComboBox_SuffixRename);
	}

	if (Button_AddPrefix)
	{
		Button_AddPrefix->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::AddPrefix);
	}

	if (Button_AddSuffix)
	{
		Button_AddSuffix->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::AddSuffix);
	}

	if (ComboBox_Suffixes)
	{
		FillCombobox(ComboBox_Suffixes);
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

	FString FileName = "BatchRename";
	GenerateFileName(FileName);
	SaveToLogFile("Start batch renaming\n", FileName);
	UE_LOG(LogRenameUtility, Display, TEXT("Create batch rename log file %s."), *FileName);

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

		if (ComboBox_SuffixRename->GetSelectedIndex() != 0)
		{
			const FString& SelectedOption = ComboBox_SuffixRename->GetSelectedOption();
			const FString Suffix = SelectedOption.Contains("_") ? SelectedOption : "_" + SelectedOption;
			FinalName.Append(Suffix);
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

void URenameUtilityWidget::AddPrefix()
{
	if (PrefixesMapIsEmpty()) return;

	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	FScopedSlowTask AddPrefixProgress(SelectedAssets.Num(),
	                                  FText::FromString(AddPrefixMessage));
	AddPrefixProgress.MakeDialog();

	FString FileName = "AddPrefix";
	GenerateFileName(FileName);
	SaveToLogFile("Start adding prefixes\n", FileName);
	UE_LOG(LogRenameUtility, Display, TEXT("Create add prefix log file %s."), *FileName);

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

void URenameUtilityWidget::AddSuffix()
{
	if (ComboBox_Suffixes->GetSelectedIndex() == 0 || SuffixesArray.Num() == 0) return;

	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	FScopedSlowTask AddSuffixProgress(SelectedAssets.Num(),
	                                  FText::FromString(AddSuffixMessage));
	AddSuffixProgress.MakeDialog();

	FString FileName = "AddSuffix";
	GenerateFileName(FileName);
	SaveToLogFile("Start adding suffixes\n", FileName);
	UE_LOG(LogRenameUtility, Display, TEXT("Create add suffix log file %s."), *FileName);

	for (UObject* Asset : SelectedAssets)
	{
		const FString UpdatedMessage = AddSuffixMessage + FString::Printf(
			TEXT(" %d/%d"),
			SelectedAssets.IndexOfByKey(Asset),
			SelectedAssets.Num());
		AddSuffixProgress.EnterProgressFrame(1, FText::FromString(UpdatedMessage));

		if (!ensure(Asset)) continue;

		const FString& SelectedOption = ComboBox_Suffixes->GetSelectedOption();
		const FString Suffix = SelectedOption.Contains("_") ? SelectedOption : "_" + SelectedOption;
		const FString Name = Asset->GetName();
		FString UpdatedName = Name;
		FString LogMessage = "";
		auto GenerateMessage = [&]()
		{
			LogMessage = FString::Printf(
				TEXT("Add suffix %s to %s. New name %s. %s"),
				*Suffix,
				*Name,
				*UpdatedName,
				*Asset->GetPathName());
		};

		if (Name.EndsWith(Suffix)) continue;

		for (int i = 0; i < SuffixesArray.Num(); i++)
		{
			if (Name.EndsWith(SuffixesArray[i]) && !Name.EndsWith(Suffix))
			{
				const FString OldSuffix = SuffixesArray[i].Contains("_") ? SuffixesArray[i] : "_" + SuffixesArray[i];
				UpdatedName = UpdatedName.Replace(*OldSuffix, *Suffix, ESearchCase::CaseSensitive);

				SaveToLogFile(FString::Printf(TEXT("Change suffix %s to %s"), *OldSuffix, *Suffix), FileName);
				GenerateMessage();
				break;
			}
		}

		if (!UpdatedName.EndsWith(Suffix))
		{
			UpdatedName += Suffix;
			GenerateMessage();
		}

		UEditorUtilityLibrary::RenameAsset(Asset, UpdatedName);
		SaveToLogFile(LogMessage, FileName);
	}

	SaveToLogFile("", FileName);
	SaveToLogFile("Finish adding suffixes\n", FileName);
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
