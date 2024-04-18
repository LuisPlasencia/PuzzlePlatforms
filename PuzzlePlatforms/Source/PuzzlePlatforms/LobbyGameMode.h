// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PuzzlePlatformsGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API ALobbyGameMode : public APuzzlePlatformsGameMode
{
	GENERATED_BODY()
	
public: 

	// estamos overrideando métodos de GameModeBase.h, el cual es clase padre de APuzzlePlatformsGameMode que es el gamemode por defecto, OJO: si queremos que también se ejecute el contenido de los métodos de la padre tendremos que usar SUPER()
	// la palabra virtual en realidad solo hace falta en la clase base osea en el padre de la que estamos overrideando (es redundante en la hija así que con el override ya es suficiente) 
	// la palabra override hace que el compilador compruebe que realmente hay una función en la clase padre con ese nombre que estemos overrideando (nos avisa si no existe o no estamos overrideando nada así que es buena práctica pero no es estrictamente necesario)		
	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

private:

	void StartGame();

	uint32 NumberOfPlayers = 0;

	FTimerHandle GameStartTimer;

};
