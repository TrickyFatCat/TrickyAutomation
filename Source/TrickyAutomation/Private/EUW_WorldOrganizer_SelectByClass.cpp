// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "EUW_WorldOrganizer_SelectByClass.h"

#include "Components/Button.h"
#include "Components/SinglePropertyView.h"
#include "EditorLevelLibrary.h"
#include "TrickyAutomationHelper.h"

void UEUW_WorldOrganizer_SelectByClass::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Button_Select)
	{
		Button_Select->OnClicked.Clear();
		Button_Select->OnClicked.AddUniqueDynamic(this, &UEUW_WorldOrganizer_SelectByClass::SelectByClass);
	}

	if (PropertyView_Class)
	{
		PropertyView_Class->SetObject(this);
		PropertyView_Class->SetPropertyName("Class");
	}
}

void UEUW_WorldOrganizer_SelectByClass::SelectByClass()
{
	if (!Class) return;

	UEditorLevelLibrary::ClearActorSelectionSet();
	TArray<AActor*> WorldActors = UEditorLevelLibrary::GetAllLevelActors();

	if (!TrickyAutomationHelper::ActorsSelectedInWorld(WorldActors)) return;

	for (AActor* Actor : WorldActors)
	{
		if (!ensure(Actor)) continue;

		if (!Actor->IsA(Class)) continue;

		UEditorLevelLibrary::SetActorSelectionState(Actor, true);
	}
}
