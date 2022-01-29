// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "EUW_WorldOrganizer_BatchRename.h"

#include "Components/Button.h"
#include "Components/SinglePropertyView.h"
#include "EditorLevelLibrary.h"
#include "TrickyAutomationHelper.h"

void UEUW_WorldOrganizer_BatchRename::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_Rename)
	{
		Button_Rename->OnClicked.AddUniqueDynamic(this, &UEUW_WorldOrganizer_BatchRename::RenameActors);
	}

	if (PropertyView_NewName)
	{
		PropertyView_NewName->SetObject(this);
		PropertyView_NewName->SetPropertyName("NewName");
	}
}

void UEUW_WorldOrganizer_BatchRename::RenameActors()
{
	TArray<AActor*> SelectedActors = UEditorLevelLibrary::GetSelectedLevelActors();
	auto PrintError = [](const FString& ErrorMessage)
	{
		TrickyAutomationHelper::PrintMessageOnScreen(ErrorMessage, FColor::Red);
	};

	if (SelectedActors.Num() == 0)
	{
		PrintError("There are no selected actors in the level");
		return;
	}

	if (NewName == "" || NewName.Contains(" "))
	{
		PrintError("Invalid new name");
		return;
	}

	for (AActor* Actor : SelectedActors)
	{
		if (!ensure(Actor)) continue;

		const FString Index = FString::FromInt(SelectedActors.IndexOfByKey(Actor));
		const FString UpdatedName = NewName.Contains("#")
			              ? NewName.Replace(TEXT("#"), *Index)
			              : FString::Printf(TEXT("%s_%s"), *NewName, *Index);
		
		TrickyAutomationHelper::RenameActorInWorld(Actor, UpdatedName);
	}

	NewName = "";
}
