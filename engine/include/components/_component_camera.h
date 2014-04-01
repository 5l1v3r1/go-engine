/**
 * @file
 * @brief Fichero que incluye la clase CComponent_Camera.
 */
#ifndef __COMPONENT_CAMERA_H_
#define __COMPONENT_CAMERA_H_

#include "_globals.h"
#include "components/_component.h"

/** @addtogroup Componentes */
/*@{*/

/**
 * @brief Modos de vista o *viewmodes*.
 *
 * Espacio de nombres usado para guardar las constantes y valores de los distintos
 * tipos de modos de vista (perspectiva, ortografica, ortografica a tama�o de pantalla, etc).
 */
namespace Viewmode
{
  /**
   * @brief Enum de modos de vista.
   *
   * Cada modo de vista tiene un valor asociado. Principalmente, 3 modos de vista:
   *
   * - **Perspectiva**: Se dibujar� con una proyecci�n en perspectiva.
   * - **Ortogr�fica**: Se dibujar� con una proyecci�n ortogr�fica. Los rangos ser�n [0, 1] para el eje vertical (de abajo a arriba), y [0, 1] para el eje horizontal (de derecha a izquierda).
   * - **ortogr�fica a tama�o de pantalla**: Se dibujar� con una proyecci�n ortogr�fica. Los rangos ser�n [0, ALTO_VENTANA] para el eje vertical (de abajo a arriba), y [0, ANCHO_VENTANA] para el eje horizontal (de derecha a izquierda).
   *
   * @see gSystem_Render (c�mara GUI).
   * @see http://es.wikipedia.org/wiki/Proyecci%C3%B3n_gr%C3%A1fica
   * @see http://es.wikipedia.org/wiki/Proyecci%C3%B3n_ortogonal
   * @see http://en.wikipedia.org/wiki/Perspective_(graphical)
   */
  enum viewmodes_t { perspective = 0, ortho, ortho_screen, __viewmode_not_defined };

  /**
   * @brief Nombres de viewmodes.
   *
   * Array de strings que contienen, por cada enum de viewmodes_t, una representaci�n con caracteres.
   * La representaci�n como cadena de caracteres ser� id�ntica al nombre del valor del enum, salvo para el caso especial *__viewmode_not_defined*, que ser� *"not_defined"*.
   */
  extern const char* viewmode_s[];

  /** @brief Transformar un modo de vista en una string.
  *
  * Dado un enum de un modo de vista, transforma dicho valor a un string.
  * @param c Valor del enum.
  * @return Representaci�n textual del componente.
  */
  const char* viewmode_to_string(viewmodes_t c);

  /**
   * @brief Transformar un string a un enum de un modo de vista.
   *
   * Dado un nombre, trata de transformar dicho nombre a una enum de modos de vista.
   * @param c Nombre del componente.
   * @return En caso de encontrar un modo de vista, devuelve su valor de enum. En caso contrario, devuelve *__viewmode_not_defined*.
   */
  viewmodes_t string_to_viewmode(const std::string& c);
}

/**
 * @brief Componente c�mara
 *
 * Dado un escenario tridimensional en nuestro programa, es necesario crear una manera de observar lo que ocurre dentro de dicho escenario.
 * El objetivo principal de este componente es actuar como una **ventana** a nuestro mundo. Esto es, observar el espacio y dibujarlo
 * en la ventana de nuestro programa.
 *
 * @image html components/camera/camera.jpg
 *
 * A este proceso se le conoce como renderizado. Cada objeto es dibujado a su manera, bajo sus componentes y sus funciones de dibujado. Por ello,
 * la c�mara se encargar� de dibujar el mundo tal y como es percibido por ella. Por tanto, asignada una c�mara a un objeto, bajo las propiedades del componente
 * CComponent_Transform, se dibujar�, seg�n la posici�n y la orientaci�n de la c�mara, el mundo tal y como es observado.
 *
 * No obstante, existen varios tipos de c�mara, enumerados en Viewmode::viewmodes_t. Dependiendo del tipo de vista, el escenario se dibujar� de una forma o de otra.
 *
 * Adem�s, cabe destacar que se puede escoger una porici�n de la pantalla sobre la que dibujar� la c�mara. Esta ser� nuestra *ventana* o **viewport**, que no ser�
 * m�s que un rect�ngulo dentro de nuestra ventana.
 *
 * Adem�s, toda c�mara debe estar registrada en el sistema CSystem_Render para poder escribir datos en pantalla.
 *
 * @warning No se eliminar� el objeto de la lista de c�maras. Deber� hacerse de manera manual desde el sistema CSystem_Render.
 *
 * Por ejemplo, creamos y registramos una c�mara de la siguiente manera:
 *
 @code
 CGameObject* camara_main = gGameObjects.AddGameObject("camara_main");

 camara_main->Transform()->position.y = -3.f;
 camara_main->Transform()->position.z = -10.f;
 camara_main->Camera()->far_clip = 500.f;
 camara_main->Preserve();                                   // Preservar entre escenas.
 camara_main->SetInputFunction(&Camara_main_movimiento);    // Cambiar el callback por una funci�n dada.
 gRender.AddCamera(camara_main);                            // Registrar en el sistema render.

 camara_main->Camera()->skybox_texture = "skybox1";
 @endcode
 *
 * Finalmente, cabe destacar que la c�mara puede definir un color o un cielo (*skybox*) de fondo, que servir� para dibujar contenido en el fondo de nuestro escenario.
 *
 * @see CSystem_Render
 */
class CComponent_Camera: public CComponent
{
  private:
    friend class CSystem_Render;
    //GLdouble projMatrix[16];
    glm::mat4 projMatrix;
    glm::mat4 modelViewMatrix;

    CGameObject* pivot;  // Si no, se usar� un pivote en la posici�n local (0, 0, -1)

  public:
    // ->PORHACER �Qu� se supone que hace CComponent_Camera::disable_gui?
    bool disable_gui; /**< @no_use */

    Viewmode::viewmodes_t viewmode; /**< Modo de vista. @see Viewmode::viewmodes_t*/
    viewportf_t viewport; /**< Ventana a dibujar en pantalla de la c�mara. Expresada en tama�o de pantalla con valores desde 0 hasta 1, ancho y alto. @see viewportf_t*/

    GLfloat field_of_view; /**< Campo de visi�n, expresado en grados. @see http://en.wikipedia.org/wiki/Field_of_view **/
    GLfloat near_clip;     /**< Distancia m�nima de separaci�n de dibujado frente a la c�mara. No se dibujar� nada que no sobrepase este valor con respecto a la c�mara. @see http://en.wikipedia.org/wiki/Viewing_frustum*/
    GLfloat far_clip;      /**< Distancia m�xima de dibujado frente a la c�mara. No se dibujar� nada que sobrepase este valor con respecto a la c�mara. @see http://en.wikipedia.org/wiki/Viewing_frustum*/

    colorf_t background_color; /**< Color con el que se recubrir� la ventana antes de dibujar. Si clear vale false, no se utilizar� este valor, y no se "limpiar�" el contenido de la ventana con nuevos valores. */
    bool clear;                /**< Saber si debe limpiarse la ventana antes de dibujar con el color de fondo. Si vale true, se limpiar� con background_color. Si no, no se har� nada, pudiendo dibujar encima. �til para un fondo transparente, pero puede darse el caso de dejar basura.*/

    std::string skybox_texture; /**< Nombre de la imagen a mostrar como *skybox*. Si es "", no se usar� ning�n skybox. No es el nombre del fichero, sino del recurso. @see CSystem_Resources @see CResource_Sound @see http://en.wikipedia.org/wiki/Skybox_(video_games) */

    // Fallo: no apunta correctamente a hijos de padres
    // �soluci�n? calcular la nueva posici�n o quitar esta opci�n
    std::string target;         /**< Nombre del objeto CGameObject al que apuntar� la c�mara. Debe estar registrado en el sistema CSystem_GameObject_Manager. Si es "" o no se encuentra, se usar� la orientaci�n de la c�mara para calcular el punto al que se debe mirar. */

    function_t before_render;   /**< Callback antes de renderizar todos los objetos con la c�mara actual. Si es null, no se har� nada. Puede ser �til para dibujar la misma escena de distintas maneras con varias c�maras. */
    function_t after_render;    /**< Callback posterior al renderizado de todos los objetos con la c�mara actual. Si es null, no se har� nada. Puede ser �til para dibujar la misma escena de distintas maneras con varias c�maras, pudiendo variar los datos, o deshacer modificaciones hechas con el callback before_render.*/

    // A�adir fog, niebla, hdr, cosas por el estilo aqu�

    /**
     * @brief LLama al callback before_render, siempre y cuando no sea NULL.
     *
     * Ser� llamado de manera autom�tica desde CSystem_Render.OnRender(), antes de dibujar todos los objetos de la escena desde la c�mara.
     */
    inline void BeforeRender()
    {
      if(before_render) before_render(gameObject);
    }

    /**
     * @brief LLama al callback after_render, siempre y cuando no sea NULL.
     *
     * Ser� llamado de manera autom�tica desde CSystem_Render.OnRender(), despu�s de dibujar todos los objetos de la escena desde la c�mara.
     */
    inline void AfterRender()
    {
      if(after_render) after_render(gameObject);
    }

  public:
    friend class CGameObject;

  private:
    static int GetID() { return Components::camera; }

    void parseDebug(std::string command);
    void printDebug();

  public:
    /**
     * @brief Constructor vac�o.
     */
    CComponent_Camera(){};

    /** @brief Constructor con objeto asociado.
     *
     * Asocia al objeto pasado como argumento el componente creado. Adem�s, inicializa los atributos de la clase a unos ciertos valores:
     *
     @code
     viewmode = perspective;
     field_of_view = 45.f;
     near_clip = 0.1f;
     far_clip = 200.f;
     clear = true;
     target = "";

     disable_gui = false; // �?

     viewport.x = viewport.y = 0;
     viewport.width = viewport.height = 1.f;

     background_color.r = background_color.g = background_color.b = 0;
     background_color.a = 1.f;

     before_render = after_render = NULL;

     skybox_texture = "";
     @endcode
     *
     * @param gameObject Objeto que guardar� el componente.
     */
    CComponent_Camera(CGameObject* gameObject);

    /**
     * @brief Destructor.
     *
     * @warning No se eliminar� el objeto de la lista de c�maras. Deber� hacerse de manera manual desde el sistema CSystem_Render.
     *
     * @see CSystem_Render
     */
    ~CComponent_Camera();

    /** @no_use */
    inline virtual void Set(input_t data);

    /** @no_use */
    inline virtual output_t Get();

    /**
     * @brief Cambiar el objetivo de la c�mara.
     *
     * Modifica el valor almacenado en target con el nuevo nombre del objeto a seguir. En caso de no querer seguir un objetivo, el target debe ser "".
     * @param t Nombre del nuevo objeto a seguir. Debe eestar registrado en el sistema CSystem_GameObject_Manager.
     */
	  void SetTarget(std::string t)
	  {
	    target = t;
	  }

    /**
     * @brief Cambiar el objetivo de la c�mara.
     *
     * Modifica el valor almacenado en target con el nuevo nombre del objeto a seguir, a partir de un puntero a un CGameObject.
     * @param obj Puntero del objeto a seguir. Debe estar registrado en el sistema CSystem_GameObject_Manager.
     */
    void SetTarget(CGameObject* obj);

    /**
     * @brief Quita el objetivo de la c�mara.
     *
     * Si no se quiere usar un objetivo que la c�mara deba seguir, se puede usar este m�todo para evitarlo. Es id�ntico a hacer que el target valga "".
     */
	  void UnSetTarget()
	  {
	    SetTarget("");
	  }

  private:
    void ApplyChanges();
    void SetViewport();
    void SetUp();
    void Clear();

    bool DrawSkybox();
};

/*@}*/

#endif /* __COMPONENT_CAMERA_H_ */
