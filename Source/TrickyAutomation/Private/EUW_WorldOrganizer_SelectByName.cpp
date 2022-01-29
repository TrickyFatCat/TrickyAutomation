// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "EUW_WorldOrganizer_SelectByName.h"

#include "TrickyAutomationHelper.h"
#include "Components/Button.h"
#include "Components/SinglePropertyView.h"
#include "EditorLevelLibrary.h"

void UEUW_WorldOrganizer_SelectByName::NativePreConstruct()
{
	Super::NativePreConstruct();

	auto InitPropertyView = [&](USinglePropertyView* PropertyView, const FName& PropertyName)
	{
		if (!PropertyView) return;

		PropertyView->SetObject(this);
		PropertyView->SetPropertyName(PropertyName);
	};

	if (Button_Select)
	{
		Button_Select->OnClicked.Clear();
		Button_Select->OnClicked.AddUniqueDynamic(this, &UEUW_WorldOrganizer_SelectByName::SelectByName);
	}

	InitPropertyView(PropertyView_SearchPattern, "SearchPattern");
	InitPropertyView(PropertyView_SearchCase, "SearchCase");
}

void UEUW_WorldOrganizer_SelectByName::SelectByName()
{
	if (!TrickyAutomationHelper::StringIsValid(SearchPattern, "search pattern")) return;

	UEditorLevelLibrary::ClearActorSelectionSet();
	TArray<AActor*> SelectedActors = TArray<AActor*>();
	TArray<AActor*> WorldActors = UEditorLevelLibrary::GetAllLevelActors();

	for (AActor* Actor : WorldActors)
	{
		if (!ensure(Actor)) continue;

		if (!Actor->GetName().Contains(SearchPattern)) continue;
		
		UEditorLevelLibrary::SetActorSelectionState(Actor, true);
	}
}
