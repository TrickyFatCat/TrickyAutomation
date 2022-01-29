// Copyright (c) 2022 Artyom "Tricky Fat Cat" Volkov (tricky.fat.cat@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "EUW_WorldOrganizer_BatchRename.generated.h"

class UButton;
class USinglePropertyView;

/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API UEUW_WorldOrganizer_BatchRename : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_Rename = nullptr;
	
	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_NewName = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Rename")
	FString NewName;

	UFUNCTION(CallInEditor)
	void RenameActors();
};
