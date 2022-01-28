// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "UtilityWidget_WorldOrganizer_FindAndReplace.generated.h"

class UButton;
class USinglePropertyView;

/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API UUtilityWidget_WorldOrganizer_FindAndReplace : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

#pragma region Find&Replace

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_FindAndReplace = nullptr;

	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_SearchPattern = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_ReplacePattern = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_SearchCase = nullptr;

	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_SearchSelected = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FindAndReplace")
	FString SearchPattern;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FindAndReplace")
	FString ReplacePattern;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FindAndReplace")
	TEnumAsByte<ESearchCase::Type> SearchCase = ESearchCase::CaseSensitive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FindAndReplace")
	bool bSearchAmongSelected = false;
	
	UFUNCTION(CallInEditor)
	void FindAndReplace();
};
