// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "EUW_AssetLibrary_Rename.h"

#include "EditorUtilityLibrary.h"
#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Components/SinglePropertyView.h"
#include "TrickyAutomationHelper.h"


void UEUW_AssetLibrary_Rename::NativePreConstruct()
{
	Super::NativePreConstruct();

	auto FillCombobox = [&](UComboBoxString* ComboBox)
	{
		if (!ComboBox) return;

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

	auto InitPropertyView = [&](USinglePropertyView* PropertyView, const FName& PropertyName)
	{
		if (!PropertyView) return;

		PropertyView->SetObject(this);
		PropertyView->SetPropertyName(PropertyName);
	};

	if (Button_RenameAssets)
	{
		Button_RenameAssets->OnClicked.AddUniqueDynamic(this, &UEUW_AssetLibrary_Rename::BatchRename);
	}

	InitPropertyView(PropertyView_NewName, "NewName");
	FillCombobox(ComboBox_SuffixRename);

	if (Button_AddPrefix)
	{
		Button_AddPrefix->OnClicked.AddUniqueDynamic(this, &UEUW_AssetLibrary_Rename::AddPrefix);
	}

	if (Button_AddSuffix)
	{
		Button_AddSuffix->OnClicked.AddUniqueDynamic(this, &UEUW_AssetLibrary_Rename::AddSuffix);
	}

	FillCombobox(ComboBox_Suffixes);

	if (Button_FindAndReplace)
	{
		Button_FindAndReplace->OnClicked.AddUniqueDynamic(this, &UEUW_AssetLibrary_Rename::FindAndReplace);
	}

	InitPropertyView(PropertyView_SearchPattern, "SearchPattern");
	InitPropertyView(PropertyView_ReplacePattern, "ReplacePattern");
	InitPropertyView(PropertyView_SearchCase, "SearchCase");
}

void UEUW_AssetLibrary_Rename::BatchRename()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	if (!TrickyAutomationHelper::AssetsSelectedInLibrary(SelectedAssets)) return;

	if (NewName == "" || NewName.Contains(" "))
	{
		TrickyAutomationHelper::PrintMessageOnScreen("Invalid new name", FColor::Red);
		return;
	}

	uint32 Counter = 0;

	FScopedSlowTask RenameProgress(SelectedAssets.Num(),
	                               FText::FromString("Renaming assets..."));
	RenameProgress.MakeDialog();

	FString FileName = "BatchRename";
	TrickyAutomationHelper::CreateLogFile(FileName, "Start batch renaming\n");

	for (UObject* Asset : SelectedAssets)
	{
		TrickyAutomationHelper::UpdateSlowTaskProgress(RenameProgress, SelectedAssets, Asset);

		if (!ensure(Asset)) continue;

		const FString OldName = Asset->GetName();
		Counter++;
		FString FinalName = NewName + FString::Printf(TEXT("_%d"), Counter);

		if (!PrefixesMapIsEmpty())
		{
			const UClass* AssetClass = Asset->GetClass();

			if (!PrefixesMap.Find(AssetClass))
			{
				TrickyAutomationHelper::SaveToLogFile(
					FString::Printf(TEXT("Can't find prefix for class %s"), *AssetClass->GetName()),
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
		TrickyAutomationHelper::SaveToLogFile(LogMessage, FileName);
	}

	TrickyAutomationHelper::SaveToLogFile("", FileName);
	TrickyAutomationHelper::SaveToLogFile("Finish batch renaming", FileName);
	NewName = "";
}

void UEUW_AssetLibrary_Rename::AddPrefix()
{
	if (PrefixesMapIsEmpty()) return;

	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	if (!TrickyAutomationHelper::AssetsSelectedInLibrary(SelectedAssets)) return;

	FScopedSlowTask AddPrefixProgress(SelectedAssets.Num(),
	                                  FText::FromString("Adding prefixes..."));
	AddPrefixProgress.MakeDialog();

	FString FileName = "AddPrefix";
	TrickyAutomationHelper::CreateLogFile(FileName, "Start adding prefixes\n");

	for (UObject* Asset : SelectedAssets)
	{
		TrickyAutomationHelper::UpdateSlowTaskProgress(AddPrefixProgress, SelectedAssets, Asset);

		if (!ensure(Asset)) continue;

		const UClass* AssetClass = Asset->GetClass();

		if (!PrefixesMap.Find(AssetClass))
		{
			TrickyAutomationHelper::SaveToLogFile(
				FString::Printf(TEXT("Can't find prefix for class %s"), *AssetClass->GetName()),
				FileName);
			continue;
		}

		FString Prefix = PrefixesMap[AssetClass];
		GeneratePrefix(Prefix);

		const FString Name = Asset->GetName();

		if (!Name.StartsWith(Prefix, ESearchCase::CaseSensitive))
		{
			UEditorUtilityLibrary::RenameAsset(Asset, Prefix + Name);
			TrickyAutomationHelper::SaveToLogFile(
				FString::Printf(
					TEXT("Add prefix %s to %s, new name is %s. %s"),
					*Prefix,
					*Name,
					*Asset->GetName(),
					*FPaths::GetPath(Asset->GetPathName())),
				FileName);
		}
	}

	TrickyAutomationHelper::SaveToLogFile("", FileName);
	TrickyAutomationHelper::SaveToLogFile("Finish adding prefixes", FileName);
}

bool UEUW_AssetLibrary_Rename::PrefixesMapIsEmpty() const
{
	if (PrefixesMap.Num() == 0)
	{
		UE_LOG(LogRenameUtility, Error, TEXT("Prefixes map is empty, can't add prefixes."));
	}

	return PrefixesMap.Num() == 0;
}

void UEUW_AssetLibrary_Rename::GeneratePrefix(FString& Prefix)
{
	if (Prefix.Contains("_")) return;

	Prefix += "_";
}

void UEUW_AssetLibrary_Rename::AddSuffix()
{
	if (ComboBox_Suffixes->GetSelectedIndex() == 0 || SuffixesArray.Num() == 0)
	{
		TrickyAutomationHelper::PrintMessageOnScreen("Please choose suffix in a dropdown menu", FColor::Red);
		return;
	}

	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	if (!TrickyAutomationHelper::AssetsSelectedInLibrary(SelectedAssets)) return;

	FScopedSlowTask AddSuffixProgress(SelectedAssets.Num(),
	                                  FText::FromString("Adding suffixes..."));
	AddSuffixProgress.MakeDialog();

	FString FileName = "AddSuffix";
	TrickyAutomationHelper::CreateLogFile(FileName, "Start adding suffixes\n");

	for (UObject* Asset : SelectedAssets)
	{
		TrickyAutomationHelper::UpdateSlowTaskProgress(AddSuffixProgress, SelectedAssets, Asset);

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

				TrickyAutomationHelper::SaveToLogFile(
					FString::Printf(TEXT("Change suffix %s to %s"), *OldSuffix, *Suffix),
					FileName);
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
		TrickyAutomationHelper::SaveToLogFile(LogMessage, FileName);
	}

	TrickyAutomationHelper::SaveToLogFile("", FileName);
	TrickyAutomationHelper::SaveToLogFile("Finish adding suffixes\n", FileName);
}

void UEUW_AssetLibrary_Rename::FindAndReplace()
{
	if (SearchPattern == "" || SearchPattern == " " || SearchPattern.Contains(" "))
	{
		TrickyAutomationHelper::PrintMessageOnScreen("Invalid search pattern", FColor::Red);
		return;
	}

	if (ReplacePattern.Contains(" ") || ReplacePattern == SearchPattern)
	{
		TrickyAutomationHelper::PrintMessageOnScreen("Invalid replace pattern", FColor::Red);
		return;
	}

	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();

	if (!TrickyAutomationHelper::AssetsSelectedInLibrary(SelectedAssets)) return;

	FScopedSlowTask FindAndReplaceProgress(SelectedAssets.Num(), FText::FromString("Replacing..."));
	FindAndReplaceProgress.MakeDialog();

	FString FileName = "FindAndReplace";
	const FString Message = FString::Printf(
		TEXT("Start searching %s and replacing with %s\n"),
		*SearchPattern,
		*ReplacePattern);
	TrickyAutomationHelper::CreateLogFile(FileName, Message);
	FString LogMessage = "";

	for (UObject* Asset : SelectedAssets)
	{
		if (!ensure(Asset)) continue;

		TrickyAutomationHelper::UpdateSlowTaskProgress<UObject*>(FindAndReplaceProgress, SelectedAssets, Asset);

		const FString Name = Asset->GetName();

		if (!Name.Contains(SearchPattern))
		{
			LogMessage = FString::Printf(
				TEXT("Can't find pattern %s in asset %s in %s"),
				*SearchPattern,
				*Name,
				*FPaths::GetPath(Asset->GetPathName()));
			TrickyAutomationHelper::SaveToLogFile(LogMessage, FileName);
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
		TrickyAutomationHelper::SaveToLogFile(LogMessage, FileName);
		UE_LOG(LogTrickyAutomation, Warning, TEXT("%s"), *Message);
	}

	TrickyAutomationHelper::SaveToLogFile("", FileName);
	TrickyAutomationHelper::SaveToLogFile("Finish replacing\n", FileName);
	UE_LOG(LogTrickyAutomation, Display, TEXT("Finish replacing"));
}
