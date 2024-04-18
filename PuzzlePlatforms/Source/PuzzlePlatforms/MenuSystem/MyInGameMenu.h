// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyMenuWidget.h"
#include "MyInGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMyInGameMenu : public UMyMenuWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize();  
	
private:  

	UPROPERTY(meta = (BindWidget)) 
	class UButton* CancelButton;

	UPROPERTY(meta = (BindWidget)) 
	class UButton* QuitButton;

	UFUNCTION()
	void CancelPressed();

	UFUNCTION()
	void QuitPressed();


};
