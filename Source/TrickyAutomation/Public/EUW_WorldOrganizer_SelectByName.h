// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "EUW_WorldOrganizer_SelectByName.generated.h"

class UButton;
class USinglePropertyView;
/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API UEUW_WorldOrganizer_SelectByName : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Select = nullptr;

	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_SearchPattern = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_SearchCase = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SelectByName")
	FString SearchPattern;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SelectByName")
	TEnumAsByte<ESearchCase::Type> SearchCase;
	
	UFUNCTION(CallInEditor)
	void SelectByName();
};
