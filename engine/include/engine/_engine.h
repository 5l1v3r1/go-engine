/**
 * @file
 * @brief Fichero que incluye la clase CEngine, y su instancia global gEngine.
 */

#ifndef __ENGINE_H_
#define __ENGINE_H_

#include "_globals.h"
#include "engine/_instance.h"

#include "systems/_render.h"

/** @addtogroup Otros */
/*@{*/

/**
 * @brief Motor gr�fico.
 *
 * Representa el conjunto de estancias del juego (los sistemas van aparte).
 * En este caso, el motor gr�fico se encargar� de gestionar la ejecuci�n y flujo principal del
 * juego. En caso de un cambio de nivel o de estancia (CInstance), ser� esta clase la encargada de
 * gestionar dicho cambio.
 *
 * El objeto de esta clase (gEngine), ser� el encargado de gestionar las estancias (CInstance) o niveles.
 * Para ello, dispone de un mapa de estancias, cada una caracterizada por un nombre (string). Desde cualquier
 * estancia, sabiendo el nombre de alguna otra, se podr� pedir un cambio de estancia, simulando un
 * cambio de nivel, por lo que se cargar�n nuevos recursos, nuevos objetos, se reiniciar�n sistemas, etc.
 *
 * @note **Nota**: en caso de querer a�adir soporte multihilo, solo bastar�a con modificar parte e esta clase, y parte de CInstance,
 * @note por lo que no deber�a de haber grandes cambios.
 *
 * @warning �sese el objeto gEngine para utilizar los m�todos de este clase, para que repercutan en tiempo de ejecuci�n.
 *
 * @see gEngine
 * @see CInstance
 */
class CEngine
{
  protected:
    //vector<CInstance*> instances;
    std::map<std::string, CInstance*> instances;
    std::string title;
    bool running;

    std::string current_instance;

    // Arguments
    std::vector<std::string> arguments;

  protected:
    void RemoveAllInstances();

  public:
    /**
     * @brief Constructor.
     *
     * Construye el motor gr�fico, y pone de t�tulo a la ventana "GO-Engine".
     */
    CEngine();
    /**
     * @brief Destructor.
     *
     * Destruye el motor gr�fico.
     */
    virtual ~CEngine(){};

    /**
     * @brief Iniciar el motor.
     *
     * Inicia el motor gr�fico, por lo que iniciar� *SDL2*, los sistemas definidos e iniciar� las funciones
     * User_Engine_Loader() (icono, t�tulo, etc.) y User_Instance_Loader() (cargar estancias). Adem�s, esta clase
     * conservar� los argumentos pasados al programa. Ser� llamado al iniciar OnExecute().
     *
     * @param argc Cantidad de argumentos del programa. Se corresponde por el argumento definido por la funci�n main().
     * @param argv Array de argumentos del programa. Se corresponde por el argumento definido por la funci�n main().
     * @return Retorna true si se inicia correctamente, false en caso contrario.
     *
     * @see CSystem
     * @see User_Engine_Loader()
     * @see User_Instance_Loader()
     */
    virtual bool Init(int argc, char* argv[]);
    /**
     * @brief Cerrar el motor.
     *
     * Cierra el motor gr�fico, cerrando la librer�a *SDL2*, cerrando los sitemas, y eliminando todas las estancias.
     * Ser� llamado al acabar OnExecute().
     */
    virtual void Close();

    /**
     * @brief Comenzar ejecuci�n
     *
     * Comienza la ejecuci�n del motor gr�fico. Primero el motor con Init(), y luego ejecuta la primer estancia a�adida con la funci�n
     * AddInstance(), usada desde User_Instance_Loader(). Finalmente, despu�s de haber ejecutado las estancias necesarias, se cerrar� el motor, llamando
     * a la funci�n close. Ser� llamado desde la funci�n main().
     *
     * @param argc Cantidad de argumentos. Procedente de la funci�n main().
     * @param argv Argumentos. Procedente de la funci�n main().
     * @return Estado de ejecuci�n. Retorna siempre 0.
     */
    virtual int OnExecute(int argc, char* argv[]);

    // Should be 32x32 RGBA format
    /**
     * @brief Cambiar icono del motor.
     *
     * Cambia el icono del motor gr�fico por uno personalizado. Debe ser una imagen de tama�o 32x32, con formato RGBA, preferiblemente en formato tga.
     * Se debe especificar la ruta completa al icono, por lo general, debe estar en data/engine
     * @param icon_name
     */
    void SetIcon(std::string icon_name);

    /**
     * @brief Obtener t�tulo.
     *
     * Devuelve el t�tulo del motor gr�fico (t�tulo de la ventana).
     *
     * @return String que contiene el t�tulo de la ventana.
     */
    std::string GetTitle()
    {
      return title;
    }

    /**
     * @brief Cambiar t�tulo.
     *
     * Cambia el t�tulo de la ventana de la aplicaci�n.
     * @param text Nuevo t�tulo.
     */
    void SetTitle(const char* text)
    {
      if(text)
        title = text;

      SDL_SetWindowTitle(gSystem_Render.window, title.c_str());
    }

    /**
     * @brief A�adir una nueva estancia.
     *
     * Esta funci�n a�ade una nueva estancia al motor gr�fico, dada una funci�n **callback** del formato
     @code
     bool callback(void);
     @endcode
     *
     * dado un nombre de estancia (identificador �nico) y la ruta a un fichero de recursos, con extensi�n .rc, que
     * contendr� los recursos necesarios para la estancia (sonidos, modelos, texturas...), generalmente en
     * la ruta <i>/data/resources</i>. El identificador de estancia ser� usado para hacer referencia a otras
     * estancias (cargar el "nivel2" desde el *"nivel1"*), y el callback se usar� para **cargar los objetos de dicha estancia**.
     * V�anse los ejemplos de prueba de la carpeta "user".
     *
     * @warning La primera estancia a�adida ser� la primera estancia cargada del programa.
     *
     * @param load_gameObject_function Callback de la funci�n para cargar los objetos de juego. Debe ser del tipo *bool callback(void)*
     * @param resource_file Ruta al fichero de recursos a cargar para dicha estancia.
     * @param instance_name Nombre de la estancia. Debe ser un identificador �nico.
     *
     * @see user/include/gameObjects.h
     * @see user/src/gameObjects.cpp
     * @see user/include/instances.h
     * @see user/src/instances.cpp
     * @see CSystem_Resources
     */
    void AddInstance(fboolpointer load_gameObject_function, std::string resource_file, std::string instance_name);

    /**
     * @brief Fotogramas por segundo.
     *
     * Retorna la cantidad de fotogramas por segundo. Ser� calculado cada 10 milisegundos.
     * @return Fotogramas por segundo de los �ltimos 10 milisegundos.
     */
    GLdouble fps()
    {
      return instances[current_instance]->fps();
    }

    /**
     * @brief Instancia ejecutada actualmente.
     *
     * Devuelve un puntero del tipo CInstance que apunta a la estancia que est� siendo ejecutada actualmente.
     * Es mejor usar GetCurrentInstance() si s�lo se desea saber el nombre.
     *
     * @see CInstance
     * @return Puntero a la estancia ejecutada actualmente.
     */
    CInstance* CurrentInstance()
    {
      return instances[current_instance];
    }

    /**
     * @brief Argumentos del programa.
     *
     * Obtener una referencia a un vector de argumentos (string) pasados al programa como argumentos.
     * Puede ser de utilidad para ciertos casos (depuraci�n, etc). Si un argumento empieza por un "-",
     * ser� ejecutado como un comando de consola despu�s de haber cargado los sistemas, y antes de
     * haber cargado la primera estancia.
     *
     * @return Referencia a un vector de argumentos.
     */
    std::vector<std::string>& Arguments()
    {
      return arguments;
    }

    // Exits without saving.
    // Should never be used
    /**
     * @brief Abortar la ejecuci�n.
     *
     * Aborta la ejecuci�n del programa, terminando bruscamente en cuanto es llamada. Esto hace que los sistemas,
     * objetos, recursos... no sean liberados correctamente. Si se quiere terminar la ejecuci�n correctamente,
     * use Exit().
     *
     * @warning No debe de usar esta funci�n salvo que sea estrictamente necesario (fallos dr�sticos).
     */
    void Abort()
    {
      exit(0);
    }

    /**
     * @brief Termina la ejecuci�n.
     *
     * Termina la ejecuci�n del programa, terminando primero la ejecuci�n de la estancia actual, para posteriormente
     * liberar recursos, cerrar sistemas, etc.
     */
    void Exit()
    {
      running = false;
      if(instances.find(current_instance) != instances.end())
        instances[current_instance]->i_running = false;
    }

    /**
     * @brief Nombre de estancia ejecutada actualmente.
     *
     * Devuelve el identificador de la estancia que se est� ejecutando actualmente.
     * @return Nombre de la estancia ejecutada actualmente. En caso de que no haya ninguna cargada, devuelve "".
     */
    std::string GetCurrentInstance()
    {
      return current_instance;
    }

    /**
     * @brief Nombre de la estancia que ser� ejecutada despu�s de la actual.
     *
     * Devuelve el identificador de la estancia que ser� ejecutada despu�s de la actual.
     *
     * @return Nombre de la estancia que ser� ejecutada despu�s que la actual. Si no se ha planeado ninguna con SetNextInstance, devolver� "".
     */
    std::string GetNextInstance()
    {
      if(instances.find(current_instance) != instances.end())
        return instances[current_instance]->next_instance;

      return "";
    }

    /**
     * @brief Cambiar de estancia.
     *
     * Cambia la pr�xima estancia a ejecutar, y finaliza la estancia actual. B�sicamente, es un "cambio de nivel".
     *
     * @param instance Nombre de la nueva estancia a ejecutar. Debe existir dicha estancia.
     */
    void NextInstance(std::string instance)
    {
      if(instances.find(current_instance) != instances.end())
      {
        instances[current_instance]->NextInstance(instance);
      }
    }

    /**
     * @brief Cambiar de estancia.
     *
     * Cambia la pr�xima estancia a ejecutar, pero **no** finaliza la estancia actual.
     *
     * @param instance Nombre de la pr�xima estancia a ejecutar. Debe existir dicha estancia.
     */
    void SetNextInstance(std::string instance)
    {
      if(instances.find(current_instance) != instances.end())
        instances[current_instance]->SetNextInstance(instance);
    }

    /**
     * @brief Terminar la estancia actual.
     *
     * Finaliza la ejecuci�n de la estancia actual. Si se ha definido una pr�xima estancia con
     * SetNextInstance(), se ejecutar� dicha estancia. En caso contrario, finalizar� la ejecuci�n del motor gr�fico.
     */
    void EndInstance()
    {
      instances[current_instance]->EndInstance();
    }
};

/**
 * @brief Objeto global del motor gr�fico.
 *
 * Para referirse al motor, basta con usar este objeto. Debe ser el �nico objeto de su tipo.
 *
 * @see CEngine
 */
extern CEngine gEngine;

/*@}*/

#endif /* __ENGINE_H_ */
