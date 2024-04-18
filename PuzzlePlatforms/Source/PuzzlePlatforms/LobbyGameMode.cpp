// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "TimerManager.h"
#include "PuzzlePlatformsGameInstance.h"

// Esta clase (world settings) tiene como padre el game mode base (project settings) y tenemos puesto que overridee al game mode base en el mapa lobby así que también se ejecutan las cosas del game mode base
// en el game mode podemos controlar los jugadores que se unen a nuestra sesión (rules)
// OJO: EL GAME MODE EXISTIRÁ SOLO EN EL SERVIDOR (el game mode lo usaremos para dejar entrar jugadores, moverlos o gestionarlos) (OTROS JUGADORES NO pueden acceder a este game mode ya que se han UNIDO a la sesión desde fuera (ClientTravel() y ServerTravel()))

// podemos buscar en AGameModeBase los métodos virtual y overridearlos. (la palabra virtual se ejecute el código de la función en la clase hija que la overridea si así fuera (OJO: y también en la padre si usamos la palabra SUPER()))
// A virtual function is a member function which is declared within a base class and is re-defined (overridden) by a derived class.
// la palabra virtual en realidad solo hace falta en la clase base osea en el padre de la que estamos overrideando
// la palabra override hace que el compilador compruebe que realmente hay una función en la clase padre con ese nombre que estemos overrideando (nos avisa si no existe o no estamos overrideando nada así que es buena práctica pero no es estrictamente necesario)


// métodos virtual que overrideamos siempre conviene llamar a Super si no, estamos reemplazando totalmente el código y dejará de hacer lo que debe hacer la función
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    ++NumberOfPlayers;
    if (NumberOfPlayers >= 2)
    {
        GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame, 10);  // timer de 10 segundos antes de empezar el game por si alguien más se quiere unir
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    --NumberOfPlayers;
}

void ALobbyGameMode::StartGame()
{
    auto GameInstance = Cast<UPuzzlePlatformsGameInstance>(GetGameInstance());  // puntero - game mode hereda de Actor que tiene GetGameInstance
    
    if (GameInstance == nullptr) return;

    GameInstance->StartSession();  // una vez empezamos la sesión, esta se considerará en progreso y no en espera. Por ello dejará de recibir jugadores y dejará de ser visible también.

    UWorld* World = GetWorld();
    if (!ensure (World != nullptr)) return;

    bUseSeamlessTravel =  true;  // Si no hacemos semaless travel, el servidor desconecta a todos los clientes y luego los mueve (mover del mapa al nuevo = tener dos mapas en memoria = mal performance), esto provoca un freeze muy poco estético (costoso en memoria). Es mejor hacer un seamless travel en el que se carga un mapa intermedio (transition map o loading screen) y antes de transicionar al mapa indicado se saca de memoria el mapa anterior para que todo sea más smooth. (https://docs.unrealengine.com/5.0/en-US/travelling-in-multiplayer-in-unreal-engine/)
    // OJO: el transition map se puede cambiar en Settings -> Default Maps -> transition map
    World->ServerTravel("/Game/PuzzlePlatforms/Maps/Game?listen");  // el listen del URL es para permitir conexiones de afuera como servidor. Permitir que otros jugadores se unan ya que estamos escuchando solicitudes

}
