// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "EUW_WorldOrganizer_FindAndReplace.h"

#include "EditorLevelLibrary.h"
#include "TrickyAutomationHelper.h"
#include "Components/Button.h"
#include "Components/SinglePropertyView.h"

void UEUW_WorldOrganizer_FindAndReplace::NativePreConstruct()
{
	Super::NativePreConstruct();

	auto InitPropertyView = [&](USinglePropertyView* PropertyView, const FName& PropertyName)
	{
		if (!PropertyView) return;

		PropertyView->SetObject(this);
		PropertyView->SetPropertyName(PropertyName);
	};

	if (Button_FindAndReplace)
	{
		Button_FindAndReplace->OnClicked.AddUniqueDynamic(this, &UEUW_WorldOrganizer_FindAndReplace::FindAndReplace);
	}

	InitPropertyView(PropertyView_SearchPattern, "SearchPattern");
	InitPropertyView(PropertyView_ReplacePattern, "ReplacePattern");
	InitPropertyView(PropertyView_SearchCase, "SearchCase");
	InitPropertyView(PropertyView_SearchSelected, "bSearchAmongSelected");
}

void UEUW_WorldOrganizer_FindAndReplace::FindAndReplace()
{
	if (TrickyAutomationHelper::StringIsValid(SearchPattern, "search pattern")) return;

	if (TrickyAutomationHelper::StringIsValid(ReplacePattern, "replace pattern")) return;

	TArray<AActor*> WorldActors = bSearchAmongSelected
		                              ? UEditorLevelLibrary::GetSelectedLevelActors()
		                              : UEditorLevelLibrary::GetAllLevelActors();

	if (bSearchAmongSelected && !TrickyAutomationHelper::ActorsSelectedInWorld(WorldActors)) return;

	for (AActor* Actor : WorldActors)
	{
		if (!ensure(Actor)) continue;

		FString Name = Actor->GetName();

		if (!Name.Contains(SearchPattern, SearchCase)) continue;

		Name = Name.Replace(*SearchPattern, *ReplacePattern, SearchCase);

		TrickyAutomationHelper::RenameActorInWorld(Actor, Name);
	}
}
