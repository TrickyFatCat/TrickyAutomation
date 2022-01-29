// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "EUW_WorldOrganizer_MoveTo.generated.h"

class UButton;
class USinglePropertyView;

/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API UEUW_WorldOrganizer_MoveTo : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_MoveTo = nullptr;

	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_NewPath = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MoveTo")
	FString NewPath;

	UFUNCTION(CallInEditor)
	void MoveTo();
};