// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "AssetCleanerUtilityWidget.generated.h"

class UButton;
class USinglePropertyView;

/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API UAssetCleanerUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_DeleteAssets = nullptr;

	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_MoveToBin = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DeleteAssets")
	bool bMoveToBinFolder = false;

	UFUNCTION(CallInEditor)
	void DeleteUnusedAssets();
};
