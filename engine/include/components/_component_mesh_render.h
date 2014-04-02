/**
 * @file
 * @brief Fichero que incluye la clase CComponent_Mesh_Render.
 */

#ifndef __COMPONENT_MESH_RENDER_H_
#define __COMPONENT_MESH_RENDER_H_

#include "_globals.h"
#include "components/_component.h"

/** @addtogroup Componentes */
/*@{*/

/**
 * @brief Componente textura GUI
 *
 * Un componente muy �til y necesario para la representaci�n gr�fica son los modelos o
 * objetos gr�ficos.
 *
 * @image html crate.png
 *
 * Un modelo es un elemento externo (un objeto 3d de Blender, Maya, 3DMax...)
 * que ha sido cargado en el motor gr�fico como un recurso.
 *
 * Por ejemplo, sup�ngase que se ha creado un objeto desde un programa externo, en un
 * formato soportado por la librer�a de *Assimp* (.3ds, .obj, etc.). Sencillamente,
 * se deber� cargar el recurso con el sistema CSystem_Resources, y utilizando el nombre del
 * recurso de modelo como el objetivo a dibujar (CComponent_Mesh_Render::mesh_name). Se extraer�
 * del componente de transformaci�n la posici�n, orientaci�n y escala para dibujar el modelo.
 *
 * Adem�s, se debe a�adir un nombre de una textura y un shader (se usar� uno por defecto si no se especifica)
 * para dibujar el modelo.
 *
 * Por tanto, si se quisiera dibujar una caja, habr�a que:
 *
 * 1. Obtener un modelo (crearlo o descargarlo) desde un programa externo.
 * 2. Cargar el modelo en el motor gr�fico (mediante ficheros .rc de recursos, v�ase CSystem_Resources).
 * 3. Asociar el componente *Mesh Render* a un objeto, junto con el nombre del modelo, su textura, y su shader.
 * 4. Dejar que el sistema dibuje el objeto seg�n crea.
 *
 * Por ejemplo, para usar un modelo, usamos:
 *
 * **Fichero *recursos.rc**:*
 @code
 # Esto l�nea es un comentario.
 # Formato de resource:
 # tipo: nombre_recurso ruta_al_fichero: [argumentos]

 mesh: mdl_hada1 data/resources/models/faerie.md2:
 # Un "mesh" no es m�s que un modelo.
 @endcode
 *
 * C�digo fuente del programa:
 @code
 CGameObject* hada1 = gGameObjects.Add("hada1"); // Crear objeto

 // Cambiar atributos de transformaci�n.
 hada1->Transform()->SetScale(0.1f, 0.1f, 0.1f);            // Reducir tama�o
 hada1->Transform()->Translate(0.f, 0.f, 10.f);             // Trasladar
 hada1->Transform()->Rotate(0.f, 90.f, 0.f);                // Rotar sobre el eje Y (gui�ada o *yaw*)

 hada1->MeshRender()->mesh_name = "mdl_hada1";              // Especificar el modelo
 hada1->MeshRender()->material_name = "texture_mdl_hada1";  // Especificar la textura
 hada1->MeshRender()->color(1.0, 0.9f, 0.9f, 1.f);          // Aplicar el color.
 @endcode
 *
 * @bug No funciona, hay que arreglar un problema con la carga de modelos de modelos de *Assimp*.
 *
 * @see CSystem_Resources
 * @see CSystem_Shader_Manager
 * @see CResource_Texture
 * @see CResource_Model
 * @see http://assimp.sourceforge.net/
 * @see http://es.wikipedia.org/wiki/Shader
 */
class CComponent_Mesh_Render: public CComponent
{
  friend class CSystem_Render;
  friend class CGameObject;

  public:
    std::string mesh_name;      /**< Nombre del recurso-modelo a usar. @see CSystem_Resources @see CResource_Model */
    std::string material_name;  /**< Nombre del recurso-textura a usar. Se usar� el mapa UV del modelo para mapear la textura. @see CSystem_Resources @see CResource_Texture */
    std::string shader_name;    /**< Nombre del shader o *programa* a usar. @see CSystem_Shader_Manager */

    //vector<string> materials;
    // Guardar colores en un vector, o algo por el estilo...
    colorf_t color;             /**< Color a aplicar sobre el modelo. Afectado incluso por la transparencia. Se trata de una aplicaci�n aditiva (simplemente colocar el color encima).*/
    GLfloat color_apply_force;  /**< Fuerza con la que se aplica el color. Si vale 1, el modelo ser� inundada por el color. Si vale 0, se a�adir� el color al modelo (adici�n).*/

    // Puede ser �til para
    function_t before_render;   /**< Callback a ejecutar antes de renderizar el modelo. Se usar� por si se quieren hacer modificaciones a bajo nivel (OpenGL). Si es NULL, no se ejecutar� nada. */
    function_t after_render;    /**< Callback a ejecutar despu�s de renderizar el modelo. Se usar� por si se quieren hacer modificaciones a bajo nivel (OpenGL). Si es NULL, no se ejecutar� nada. */

    // Modo difuso, etc etc

  private:
    static int GetID() { return Components::mesh_render; }

    void parseDebug(std::string command);
    void printDebug();

  public:
    /** @brief Constructor vac�o. */
    CComponent_Mesh_Render(){};
    /** @brief Constructor con objeto asociado.
     *
     * Asocia al objeto pasado como argumento el componente creado. Adem�s, inicializa los atributos de la clase a unos ciertos valores:
     *
     @code
     mesh_name = material_name = "";
     shader_name = "__textureShader";
     before_render = after_render = NULL;

     color(1.f, 1.f, 1.f, 1.f);
     color_apply_force = 0.f;
     @endcode
     *
     * @param gameObject Objeto que guardar� el componente.
     */
    CComponent_Mesh_Render(CGameObject* gameObject);
    /** @brief Destructor.
     * Destruye el componente. */
    ~CComponent_Mesh_Render();

  protected:
    void OnRender(glm::mat4 projMatrix, glm::mat4 modelViewMatrix);
};

/*@}*/

#endif /* __COMPONENT_MESH_RENDER_H_ */
