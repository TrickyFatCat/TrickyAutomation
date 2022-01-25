// Fill out your copyright notice in the Description page of Project Settings.


#include "RenameUtilityWidget.h"

#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "Components/Button.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopedSlowTask.h"
#include "Components/DetailsView.h"


void URenameUtilityWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_RenameAssets)
	{
		Button_RenameAssets->OnClicked.AddUniqueDynamic(this, &URenameUtilityWidget::BatchRename);
	}

	if (DetailsView_BatchRename)
	{
		DetailsView_BatchRename->SetObject(this);
		DetailsView_BatchRename->PropertiesToShow.AddUnique("NewName");
		DetailsView_BatchRename->bAllowFiltering = false;
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
	const FString FileName = "BatchRename_YYMMDD_HHMMSS";
	// TODO Add date and time to a log name
	SaveToLogFile("Start batch renaming\n", FileName);

	for (UObject* Asset : SelectedAssets)
	{
		const FString UpdatedMessage = RenameMessage + FString::Printf(
			TEXT(" %d/%d"),
			SelectedAssets.IndexOfByKey(Asset),
			SelectedAssets.Num());
		RenameSelectedAssets.EnterProgressFrame(1, FText::FromString(UpdatedMessage));

		if (ensure(Asset))
		{
			const FString OldName = Asset->GetName();
			Counter++;
			const FString FinalName = NewName + FString::Printf(TEXT("_%d"), Counter);
			UEditorUtilityLibrary::RenameAsset(Asset, FinalName);

			// TODO Add prefix
			FString LogMessage = FString::Printf(
				TEXT("Rename %s to %s in %s"),
				*OldName,
				*FinalName,
				*Asset->GetPathName());

			UE_LOG(LogRenameUtility, Warning, TEXT("%s"), *LogMessage);
			SaveToLogFile(LogMessage, FileName);
		}
	}

	SaveToLogFile("", FileName);
	SaveToLogFile("Finish batch renaming", FileName);
	NewName = "";
}

void URenameUtilityWidget::SaveToLogFile(const FString& Message, const FString& FileName)
{
	const FString LogMessage = Message == "" ? FString("\n") : FString::Printf(TEXT("YYYY-MM-DD HH:MM:SS | %s\n"), *Message);
	
	// TODO Add date and time to a message
	// TODO Add message type SUCCESS/ERROR/WARNING
	const FString LogFileName = "Log_" + FileName + ".txt";
	const FString FilePath = FPaths::ProjectDir() + "Saved/Logs/TrickyAutomation/" + LogFileName;
	FFileHelper::SaveStringToFile(LogMessage,
	                              *FilePath,
	                              FFileHelper::EEncodingOptions::AutoDetect,
	                              &IFileManager::Get(),
	                              FILEWRITE_Append);
}
