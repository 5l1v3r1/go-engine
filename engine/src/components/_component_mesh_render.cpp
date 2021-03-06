#include "components/_component_mesh_render.h"
#include "systems/_resource.h"
#include "systems/_shader.h"
#include "systems/_debug.h"
#include "systems/_other.h"

using namespace std;


//BOOST_CLASS_EXPORT_IMPLEMENT(CComponent_Mesh_Render);

CComponent_Mesh_Render::CComponent_Mesh_Render(CGameObject* gameObject) :
    CComponent(gameObject)
{
  mesh_name = material_name = "";
  shader_name = "__textureShader";
  before_render = after_render = NULL;

  //materials.resize(0);
  color(1.f, 1.f, 1.f, 1.f);
  color_apply_force = 0.f;
}

CComponent_Mesh_Render::~CComponent_Mesh_Render()
{
  //materials.resize(0);
}

void CComponent_Mesh_Render::OnRender(glm::mat4 projMatrix, glm::mat4 modelViewMatrix)
{
  if(!enabled)
    return;

  // Guardar el shader dentro del mesh render!
  //CShader* simpleShader = gSystem_Shader_Manager.GetShader(shader_name);
  //glUseProgram(simpleShader->GetProgram());
  glm::mat4 NormalMatrix = glm::transpose(glm::inverse((modelViewMatrix)));

  CShader* simpleShader = gSystem_Shader_Manager.UseShader(shader_name);

  glUniformMatrix4fv(simpleShader->GetUniformIndex("ProjMatrix"), 1, GL_FALSE,
      glm::value_ptr(projMatrix));
  glUniformMatrix4fv(simpleShader->GetUniformIndex("ModelViewMatrix"), 1, GL_FALSE,
      glm::value_ptr(modelViewMatrix));
  glUniformMatrix4fv(simpleShader->GetUniformIndex("NormalMatrix"), 1, GL_FALSE,
      glm::value_ptr(NormalMatrix));
  glUniform1i(simpleShader->GetUniformIndex("texture"), 0);

  gSystem_Math.Clamp(color_apply_force, 0.f, 1.f);


  if(before_render)
    before_render(gameObject);

  if(color.a != 1.0)
  {
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    //glEnable(GL_BLEND);
  }

  CResource_Mesh* mesh = gSystem_Resources.GetMesh(mesh_name);
  glActiveTexture(GL_TEXTURE0);
  if(mesh != gSystem_Resources.GetMesh("__MDL_ERROR")) // Cambiar el color a rojo
  {
    glBindTexture(GL_TEXTURE_2D, gSystem_Resources.GetTexture(material_name)->GetID());
    glUniform4f(simpleShader->GetUniformIndex("in_Color"), color.r, color.g, color.b, color.a);
    glUniform1f(simpleShader->GetUniformIndex("textureFlag"), 1.0f - color_apply_force);
  }
  else
  {
    glBindTexture(GL_TEXTURE_2D, gSystem_Resources.GetTexture("__TEXTURE_WHITE")->GetID());
    glUniform4f(simpleShader->GetUniformIndex("in_Color"), 1.f, 0.f, 0.56f, 1.f);
    glUniform1f(simpleShader->GetUniformIndex("textureFlag"), 1.0f);
  }

  mesh->Render();

  glDepthMask(GL_TRUE);
  //glDisable(GL_BLEND);

  if(after_render)
    after_render(gameObject);

  //glUseProgram(0);
}

void CComponent_Mesh_Render::parseDebug(string command)
{
  stringstream ss(command);
  string attrib;

  ss >> attrib;

  if(attrib == "help" or attrib == "?" or attrib == "")
  {
    printDebug();

    return;
  }

  if(attrib == "mesh_name" or attrib == "material_name" or attrib == "shader_name")
  {
    string data;
    ss >> data;

    if(ss.fail())
    {
      gSystem_Debug.console_error_msg(
          "From component %s - %s: Invalid format. Data format is: \"<atribute> <attriube type value>\"",
          gameObject->GetName().c_str(),
          Components::component_to_string((Components::components_t) GetID()));

      return;
    }

    if(attrib == "mesh_name")
      mesh_name = data;
    else if(attrib == "material_name")
      material_name = data;
    else if(attrib == "shader_name")
      shader_name = data;

    gSystem_Debug.console_msg("From component %s - %s: Set variable \"%s\" to value \"%s\".",
        gameObject->GetName().c_str(),
        Components::component_to_string((Components::components_t) GetID()), attrib.c_str(),
        data.c_str());
  }
  else if(attrib == "color")
  {
    colorf_t data;
    ss >> data;

    if(ss.fail())
    {
      gSystem_Debug.console_error_msg(
          "From component %s - %s: Invalid format. Data format is: \"<atribute> <attriube type value>\"",
          gameObject->GetName().c_str(),
          Components::component_to_string((Components::components_t) GetID()));

      return;
    }

    color = data;

    gSystem_Debug.console_msg("From component %s - %s: Set variable \"%s\" to value \"%s\".",
        gameObject->GetName().c_str(),
        Components::component_to_string((Components::components_t) GetID()), attrib.c_str(),
        data.str().c_str());
  }
  else if(attrib == "apply_force")
  {
    float data;
    ss >> data;

    if(ss.fail())
    {
      gSystem_Debug.console_error_msg(
          "From component %s - %s: Invalid format. Data format is: \"<atribute> <attriube type value>\"",
          gameObject->GetName().c_str(),
          Components::component_to_string((Components::components_t) GetID()));

      return;
    }

    color_apply_force = data;
    gSystem_Debug.console_msg("From component %s - %s: Set variable \"%s\" to value \"%s\".",
        gameObject->GetName().c_str(),
        Components::component_to_string((Components::components_t) GetID()), attrib.c_str(), data);
  }
  else
  {
    gSystem_Debug.console_error_msg("From component %s - %s: Unknow attribute \"%s\".",
        gameObject->GetName().c_str(),
        Components::component_to_string((Components::components_t) GetID()), attrib.c_str());
  }
}


void CComponent_Mesh_Render::printDebug()
{
  gSystem_Debug.console_warning_msg("Component %s uses the following attributes:",
      Components::component_to_string((Components::components_t) GetID()));
  gSystem_Debug.console_warning_msg("Attribute      Type             Value");
  gSystem_Debug.console_warning_msg("-------------------------------------");
  gSystem_Debug.console_warning_msg("mesh_name           string      %s", mesh_name.c_str());
  gSystem_Debug.console_warning_msg("material_name       string      %s", material_name.c_str());
  gSystem_Debug.console_warning_msg("shader_name         string      %s", shader_name.c_str());
  gSystem_Debug.console_warning_msg("color               colorf_t    %s", color.str().c_str());
  gSystem_Debug.console_warning_msg("color_apply_force   float       %f", color_apply_force);
}
