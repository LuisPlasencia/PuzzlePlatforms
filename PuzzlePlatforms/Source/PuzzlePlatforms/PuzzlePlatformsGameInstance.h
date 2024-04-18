// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "MenuSystem/MenuInterface.h"
#include "PuzzlePlatformsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMenuInterface  // incluyo la interfaz y la implemento
{
	GENERATED_BODY()

public:
	UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer);

	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void LoadMenu();

	UFUNCTION(BlueprintCallable)
	void InGameLoadMenu();

	UFUNCTION(Exec)   // Exec behaviour (called by the console) = Compatible with anything linked to the player controller (classes that can receive exec functions): PlayerControllers, Possessed Pawns, HUDs, Cheat Managers, Game Modes, Game Instances
	void Host(FString ServerName) override;     // CONSOLE COMMAND HOST

	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	void StartSession();

	// UFUNCTION(Exec)
	// void Join(const FString& Address) override;

	virtual void LoadMainMenu() override;

	void RefreshServerList() override;

private: 
	TSubclassOf<class UUserWidget> MenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMyMainMenu* MainMenu;

	IOnlineSessionPtr SessionInterface;    // no podemos forward declarar un TSharedPtr así que tenemos que añadir el include aquí en el header
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool Success);  // OJO: es un TMulticastDelegate con dos parámetros. No es un delegate dinámico sino estático (no necesidad de UFunction en este caso sino funciones C++ normales así que me puedo ahorrar el UFUNCTION() arriba).
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionsComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);  // para poder incorporar esta declaración tuvimos que hacer include "Interfaces/OnlineSessionInterface.h" que inicialmente estaba en el fichero cpp.
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);  // el formato del delegado (signature) se encuentra en Engine.h, podemos ir haciendole F12 a GEngine->OnNetworkFailure()

	FString DesiredServerName;
	void CreateSession();


};
