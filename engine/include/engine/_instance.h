/**
 * @file
 * @brief Fichero que incluye la clase CInstance.
 */

#ifndef __INSTANCE_H_
#define __INSTANCE_H_

#include "_globals.h"

/** @addtogroup Otros */
/*@{*/

/**
 * @brief Clase que representa a una estancia o nivel.
 *
 * Una estancia o nivel representa un escenario concreto de nuestra aplicaci�n o nivel.
 * Por ejemplo, sup�ngase el caso en el que nuestro juego esta compuesto de varios subjuegos (3 en raya,
 * ajedrez, p�ker...). Cada juego podr�a corresponderse con una estancia, y el men� de selecci�n de juego
 * tambi�n podr�a considerarse una estancia o nivel.
 *
 * As� pues, una estancia de componene de:
 * - Una serie de objetos de juego, cargados desde el callback de carga de objetos (v�ase CInstance() ).
 * - Una serie de recursos, sean texturas, modelos, sonidos... cargados directamente desde un fichero de recursos *.rc*.
 * - Una pr�xima estancia a ejecutar (si no hay pr�xima, finaliza el juego).
 *
 * Sin m�s, el motor gr�fica u objeto gEngine se encargar� de gestionar las estancias, de manera que el usuario s�lo
 * tiene que definirlas, definir la pr�xima estancia de cada una (se puede hacer de manera din�mica) y estblecer un
 * fichero de recursos a cargar.
 *
 * @see CEngine
 * @see gEngine
 *
 */
class CInstance
{
  protected:
    friend class CEngine;

    bool i_running;
    std::string next_instance;

    fboolpointer gameObjects_loader;
    std::string resource_file;

    /** FPS **/
    GLdouble FPS;
    uint frames;
    Uint32 current_time, previous_time;

    bool first_frame;

    bool Init();
      bool LoadResources();

    void Close();
      void UnLoadResources(); // se descargar�n los archivos cuando, al cargar los archivos de la proxima estancia, se demuestre que solo se descartaran los que no necesitan seguir en el motor

      std::string OnExecute();
    void OnLoop();
    void OnEvent();
    void OnRender();
    void CalculateFPS();

  public:
    /**
     * @brief Constructor principal.
     *
     * Crea una estancia. No debe llamarse a este constructor de manera din�mica, sino que
     * debe ser creada por el m�todo CEngine::AddInstance(). Se caracteriza porque se le proporciona
     * un callback de los objetos a crear, y un fichero de recursos.
     *
     * @param load_gameObject_function
     * @param resource_file
     */
    CInstance(fboolpointer load_gameObject_function, std::string resource_file);
    //~CInstance();

    GLdouble fps()
    {
      return FPS;
    }

    void NextInstance(std::string next_instance_name);
      void SetNextInstance(std::string next_instance_name);
      void EndInstance();

    //virtual void OnDebug(){};
};

/*@}*/

#endif /* __INSTANCE_H_ */
