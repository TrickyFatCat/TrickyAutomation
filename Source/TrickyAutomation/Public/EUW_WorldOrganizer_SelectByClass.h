// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "EUW_WorldOrganizer_SelectByClass.generated.h"

class UButton;
class USinglePropertyView;

/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API UEUW_WorldOrganizer_SelectByClass : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Select = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_Class = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SelectByClass")
	TSubclassOf<AActor> Class;

	UFUNCTION(CallInEditor)
	void SelectByClass();
};
