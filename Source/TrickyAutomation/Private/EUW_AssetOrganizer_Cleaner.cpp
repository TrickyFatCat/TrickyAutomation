// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "EUW_AssetsOrganizer_Cleaner.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Components/Button.h"
#include "Components/SinglePropertyView.h"
#include "TrickyAutomationHelper.h"
#include "Misc/ScopedSlowTask.h"

void UEUW_AssetsOrganizer_Cleaner::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_DeleteAssets)
	{
		Button_DeleteAssets->OnClicked.AddUniqueDynamic(this, &UEUW_AssetsOrganizer_Cleaner::DeleteUnusedAssets);
	}

	if (PropertyView_MoveToBin)
	{
		PropertyView_MoveToBin->SetObject(this);
		PropertyView_MoveToBin->SetPropertyName("bMoveToBinFolder");
	}
}

void UEUW_AssetsOrganizer_Cleaner::DeleteUnusedAssets()
{
	TArray<UObject*> SelectedAssets = UEditorUtilityLibrary::GetSelectedAssets();
	TArray<UObject*> UnusedAssets = TArray<UObject*>();

	if (!TrickyAutomationHelper::AssetsSelectedInLibrary(SelectedAssets)) return;

	FScopedSlowTask DeleteProgress(SelectedAssets.Num(), FText::FromString("Checking assets..."));
	DeleteProgress.MakeDialog();

	for (UObject* Asset : SelectedAssets)
	{
		TrickyAutomationHelper::UpdateSlowTaskProgress(DeleteProgress, SelectedAssets, Asset);
		
		if (!ensure(Asset)) continue;

		if (UEditorAssetLibrary::FindPackageReferencersForAsset(Asset->GetPathName(), true).Num() <= 0)
		{
			UnusedAssets.Add(Asset);
		}
	}

	DeleteProgress.Visibility = ESlowTaskVisibility::Invisible;

	if (UnusedAssets.Num() == 0)
	{
		TrickyAutomationHelper::PrintMessageOnScreen("All assets are in use", FColor::Green);
		return;
	}

	const FString ProgressMessage = bMoveToBinFolder ? "Moving to bin..." : "Deleting assets...";
	DeleteProgress.DefaultMessage = FText::FromString(ProgressMessage);
	DeleteProgress.TotalAmountOfWork = UnusedAssets.Num();
	DeleteProgress.CompletedWork = 0;

	FString FileName = "DeleteUnusedAssets";
	TrickyAutomationHelper::CreateLogFile(FileName, "Start deleting unused assets\n");

	FString LogMessage = "";
	auto SaveMessageToLog = [&LogMessage, &FileName]() { TrickyAutomationHelper::SaveToLogFile(LogMessage, FileName); };

	for (UObject* Asset : UnusedAssets)
	{
		TrickyAutomationHelper::UpdateSlowTaskProgress(DeleteProgress, UnusedAssets, Asset);

		if (!ensure(Asset)) return;

		const FString Name = Asset->GetName();
		const FString InitialPath = FPaths::GetPath(Asset->GetPathName());

		if (bMoveToBinFolder)
		{
			const FString NewPath = FPaths::Combine(TEXT("/Game"), TEXT("Bin"), *Name);

			if (UEditorAssetLibrary::RenameAsset(Asset->GetPathName(), NewPath))
			{
				LogMessage = FString::Printf(TEXT("Move %s to a bin folder from %s"), *Name, *InitialPath);
				SaveMessageToLog();
				UE_LOG(LogTrickyAutomation, Warning, TEXT("%s"), *LogMessage);
				continue;
			}

			LogMessage = FString::Printf(TEXT("Couldn't move %s to a bin folder from %s"), *Name, *InitialPath);
			SaveMessageToLog();
			UE_LOG(LogTrickyAutomation, Error, TEXT("%s"), *LogMessage);
			continue;
		}

		if (UEditorAssetLibrary::DeleteAsset(Asset->GetPathName()))
		{
			LogMessage = FString::Printf(TEXT("Delete %s from %s"), *Name, *InitialPath);
			SaveMessageToLog();
			UE_LOG(LogTrickyAutomation, Warning, TEXT("%s"), *LogMessage);
			continue;
		}

		LogMessage = FString::Printf(TEXT("Couldn't delet %s from %s"), *Name, *InitialPath);
		SaveMessageToLog();
		UE_LOG(LogTrickyAutomation, Error, TEXT("%s"), *LogMessage);
	}

	TrickyAutomationHelper::SaveToLogFile("", FileName);
	TrickyAutomationHelper::SaveToLogFile("Finished deleting unused assets", FileName);
}
