// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "EUW_WorldOrganizer_MoveTo.h"

#include "Components/Button.h"
#include "Components/SinglePropertyView.h"
#include "EditorLevelLibrary.h"
#include "TrickyAutomationHelper.h"

void UEUW_WorldOrganizer_MoveTo::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_MoveTo)
	{
		Button_MoveTo->OnClicked.Clear();
		Button_MoveTo->OnClicked.AddUniqueDynamic(this, &UEUW_WorldOrganizer_MoveTo::MoveTo);
	}

	if (PropertyView_NewPath)
	{
		PropertyView_NewPath->SetObject(this);
		PropertyView_NewPath->SetPropertyName("NewPath");
	}
}

void UEUW_WorldOrganizer_MoveTo::MoveTo()
{
	if (NewPath == "" || NewPath.Contains(" ") || NewPath.Contains("\\"))
	{
		TrickyAutomationHelper::PrintMessageOnScreen("Invalid path", FColor::Red);
		return;
	}
	
	TArray<AActor*> SelectedActors = UEditorLevelLibrary::GetSelectedLevelActors();

	if (!TrickyAutomationHelper::ActorsSelectedInWorld(SelectedActors)) return;

	for (AActor* Actor : SelectedActors)
	{
		if (!ensure(Actor)) continue;

		Actor->SetFolderPath(*NewPath);
	}
}
