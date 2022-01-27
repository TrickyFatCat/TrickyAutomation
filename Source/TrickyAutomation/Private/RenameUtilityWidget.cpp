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

	auto InitPropertyView = [](USinglePropertyView* PropertyView, UObject* Object, const FName& PropertyName)
	{
		PropertyView->SetObject(Object);
		PropertyView->SetPropertyName(PropertyName);
	};

	if (Button_RenameAssets)
	{
		Button_RenameAssets->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::BatchRename);
	}

	if (PropertyView_NewName)
	{
		InitPropertyView(PropertyView_NewName, this, "NewName");
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

	if (Button_FindAndReplace)
	{
		Button_FindAndReplace->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::FindAndReplace);
	}

	if (PropertyView_SearchPattern)
	{
		InitPropertyView(PropertyView_SearchPattern, this, "SearchPattern");
	}

	if (PropertyView_ReplacePattern)
	{
		InitPropertyView(PropertyView_ReplacePattern, this, "ReplacePattern");
	}

	if (PropertyView_SearchCase)
	{
		InitPropertyView(PropertyView_SearchCase, this, "SearchCase");
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

	FScopedSlowTask RenameProgress(SelectedAssets.Num(),
	                               FText::FromString("Renaming assets..."));
	RenameProgress.MakeDialog();

	FString FileName = "BatchRename";
	CreateLogFile(FileName, "Start batch renaming\n");

	for (UObject* Asset : SelectedAssets)
	{
		UpdateSlowTaskProgress(RenameProgress, SelectedAssets, Asset);

		if (!ensure(Asset)) continue;

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
			*FPaths::GetPath(Asset->GetPathName()));

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
	                                  FText::FromString("Adding prefixes..."));
	AddPrefixProgress.MakeDialog();

	FString FileName = "AddPrefix";
	CreateLogFile(FileName, "Start adding prefixes\n");

	for (UObject* Asset : SelectedAssets)
	{
		UpdateSlowTaskProgress(AddPrefixProgress, SelectedAssets, Asset);

		if (!ensure(Asset)) continue;
		
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
					*FPaths::GetPath(Asset->GetPathName())),
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
	                                  FText::FromString("Adding suffixes..."));
	AddSuffixProgress.MakeDialog();

	FString FileName = "AddSuffix";
	CreateLogFile(FileName, "Start adding suffixes\n");

	for (UObject* Asset : SelectedAssets)
	{
		UpdateSlowTaskProgress(AddSuffixProgress, SelectedAssets, Asset);

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
				*FPaths::GetPath(Asset->GetPathName()));
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

void URenameUtilityWidget::FindAndReplace()
{
	if (SearchPattern == "")
	{
		// TODO Log error
		return;
	}

	if (PrefixesMap.FindKey(SearchPattern))
	{
		// TODO Log error
		return;
	}

	if (SuffixesArray.Contains(SearchPattern))
	{
		// TODO Log error
		return;
	}

	if (ReplacePattern.Contains(" "))
	{
		// TODO Log error
		return;
	}

	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	FScopedSlowTask FindAndReplaceProgress(SelectedAssets.Num(), FText::FromString("Replacing..."));
	FindAndReplaceProgress.MakeDialog();

	FString FileName = "FindAndReplace";
	const FString Message = FString::Printf(
		TEXT("Start searching %s and replacing with %s"),
		*SearchPattern,
		*ReplacePattern);
	CreateLogFile(FileName, Message);
	FString LogMessage = "";

	for (UObject* Asset : SelectedAssets)
	{
		if (!ensure(Asset)) continue;

		UpdateSlowTaskProgress(FindAndReplaceProgress, SelectedAssets, Asset);

		const FString Name = Asset->GetName();

		if (!Name.Contains(SearchPattern))
		{
			LogMessage = FString::Printf(
				TEXT("Can't find pattern %s in asset %s in %s"),
				*SearchPattern,
				*Name,
				*FPaths::GetPath(Asset->GetPathName()));
			SaveToLogFile(LogMessage, FileName);
			UE_LOG(LogRenameUtility, Error, TEXT("%s"), *LogMessage);
			continue;
		}

		const FString& UpdatedName = Name.Replace(*SearchPattern, *ReplacePattern, SearchCase);
		UEditorUtilityLibrary::RenameAsset(Asset, UpdatedName);
		LogMessage = FString::Printf(
			TEXT("Replace %s with %s in %s. New name %s. %s"),
			*SearchPattern,
			*ReplacePattern,
			*Name,
			*UpdatedName,
			*FPaths::GetPath(Asset->GetPathName()));
		SaveToLogFile(LogMessage, FileName);
		UE_LOG(LogRenameUtility, Warning, TEXT("%s"), *Message);
	}

	SaveToLogFile("", FileName);
	SaveToLogFile("Finish replacing\n", FileName);
	UE_LOG(LogRenameUtility, Display, TEXT("Finish replacing"));
}

FString URenameUtilityWidget::GetPluginLogDir()
{
	return FPaths::ProjectLogDir() + "TrickyAutomation/";
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
	const FString FilePath = GetPluginLogDir() + LogFileName;
	FFileHelper::SaveStringToFile(LogMessage,
	                              *FilePath,
	                              FFileHelper::EEncodingOptions::AutoDetect,
	                              &IFileManager::Get(),
	                              FILEWRITE_Append);
}

void URenameUtilityWidget::CreateLogFile(FString& FileName, const FString& Message)
{
	GenerateFileName(FileName);
	SaveToLogFile(Message, FileName);
	UE_LOG(LogRenameUtility,
	       Display,
	       TEXT("Create log file %s in %s."),
	       *FileName,
	       *GetPluginLogDir());
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

void URenameUtilityWidget::UpdateSlowTaskProgress(FScopedSlowTask& SlowTask,
                                                  const TArray<UObject*>& SelectedAssets,
                                                  const UObject* CurrentAsset)
{
	const FString UpdatedMessage = SlowTask.DefaultMessage.ToString() + FString::Printf(
		TEXT(" %d/%d"),
		SelectedAssets.IndexOfByKey(CurrentAsset),
		SelectedAssets.Num());
	SlowTask.EnterProgressFrame(1, FText::FromString(UpdatedMessage));
}
