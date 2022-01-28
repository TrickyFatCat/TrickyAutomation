// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)


#include "UtilityWidget_WorldOrganizer_FindAndReplace.h"

#include "EditorLevelLibrary.h"
#include "TrickyAutomationHelper.h"
#include "Components/Button.h"
#include "Components/SinglePropertyView.h"

void UUtilityWidget_WorldOrganizer_FindAndReplace::NativePreConstruct()
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
		Button_FindAndReplace->OnClicked.AddUniqueDynamic(this, &UUtilityWidget_WorldOrganizer_FindAndReplace::FindAndReplace);
	}

	InitPropertyView(PropertyView_SearchPattern, "SearchPattern");
	InitPropertyView(PropertyView_ReplacePattern, "ReplacePattern");
	InitPropertyView(PropertyView_SearchCase, "SearchCase");
	InitPropertyView(PropertyView_SearchSelected, "bSearchAmongSelected");
}

void UUtilityWidget_WorldOrganizer_FindAndReplace::FindAndReplace()
{
	auto PrintError = [](const FString& LogMessage)
	{
		TrickyAutomationHelper::PrintMessageOnScreen(LogMessage, FColor::Red);
	};

	if (SearchPattern == "" || SearchPattern.Contains(" ") || SearchPattern == ReplacePattern)
	{
		PrintError("Invalid search pattern");
		return;
	}

	TArray<AActor*> WorldActors = bSearchAmongSelected
		                              ? UEditorLevelLibrary::GetSelectedLevelActors()
		                              : UEditorLevelLibrary::GetAllLevelActors();

	if (bSearchAmongSelected && WorldActors.Num() == 0)
	{
		PrintError("No assets selected when bSearchAmongSelected == true");
		return;
	}

	for (AActor* Actor : WorldActors)
	{
		if (!ensure(Actor)) continue;

		FString Name = Actor->GetName();

		if (!Name.Contains(SearchPattern, SearchCase)) continue;

		Name = Name.Replace(*SearchPattern, *ReplacePattern, SearchCase);

		TrickyAutomationHelper::RenameActorInWorld(Actor, Name);
	}
}
