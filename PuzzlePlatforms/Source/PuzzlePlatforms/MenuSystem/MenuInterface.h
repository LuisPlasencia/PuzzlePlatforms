// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *   Para prevenir el diamond inheritance o multiple inheritance (en el que múltiples cosas acaban teniendo el mismo padre), escribimos las funciones solo en la segunda clase
 *   y es que la clase de arriba lo único que hace es comunicarle a la interfaz lo que es (gameObject - UObject si no no funciona), pero al heredar de la interfaz no queremos heredar de múltiples gameObjects - UObjects (UMenuInterface (donde le haremos uso) & UGameInstance)
 * 	 Por eso heredamos de IMenuInterface, el cual está limpio de herencias (no tiene padres)
 */
class PUZZLEPLATFORMS_API IMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Host(FString ServerName) = 0;   // (virtual = 0) = pure virtual function = no implementation or empty implementation, the class that inherits from me will implement it for me - si no ponemos = 0 nos pedirá que la implementemos en cpp y eso mataría el propósito de la interfaz
	virtual void Join(uint32 Index) = 0;
	virtual void LoadMainMenu() = 0;
	virtual void RefreshServerList() = 0;

};
