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
		Button_RenameAssets->OnClicked.AddDynamic(this, &URenameUtilityWidget::BatchRename);
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

	if (NewName == "")
	{
		// TODO Print error
		return;
	}

	uint32 Counter = 0;

	for (UObject* Asset : SelectedAssets)
	{
		if (!ensure(Asset))
		{
			// TODO Print and log error
			continue;
		}

		Counter++;
		const FString FinalName = NewName + FString::Printf(TEXT("_%02d"), Counter);
		UEditorUtilityLibrary::RenameAsset(Asset, FinalName);

		// TODO Add prefix
		// TODO Log changes
	}

	NewName = "";
}
