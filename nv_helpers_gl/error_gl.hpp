/* Copyright (c) 2014-2018, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#ifndef NV_ERROR_INCLUDED
#define NV_ERROR_INCLUDED

#include <include_gl.h>
#include <string>

#include <nv_helpers/nvprint.hpp>

namespace nv_helpers_gl
{
  bool checkGLVersion(GLint MajorVersionRequire, GLint MinorVersionRequire);
  bool checkExtension(char const * String);

  bool checkError(const char* Title);
  bool checkNamedFramebuffer(GLuint fbo);

  enum CheckBindingBits {
    CHECKBINDING_VAO_BIT = 1<<0,
    CHECKBINDING_FBO_BIT = 1<<1,
    CHECKBINDING_PROGRAM_BIT = 1<<2,
    CHECKBINDING_TEXTURES_BIT = 1<<3,
    CHECKBINDING_SAMPLERS_BIT = 1<<4,
    CHECKBINDING_BUFFERS_BIT = 1<<5,
    CHECKBINDING_IMAGES_BIT = 1<<6,
    CHECKBINDING_XFB_BIT = 1<<7,
    CHECKBINDING_VATTRIBS_BIT = 1<<8,
    CHECKBINDING_ALL_BITS = ~0,
  };

#ifndef NDEBUG
  #define DBG_CHECKBINDINGS(bindingBits)   checkBindings(bindingBits, __FILE__, __LINE__);
#else
  #define DBG_CHECKBINDINGS(bindingBits)
#endif

  bool checkBindings(int bindingBits, const char* marker = NULL, int num=0);

}

//////////////////////////////////////////////////////////////////////////

namespace nv_helpers_gl
{


  inline bool checkNamedFramebuffer(GLuint fbo)
  {
    GLenum status = glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
    switch (status)
    {
    case GL_FRAMEBUFFER_UNDEFINED:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_UNDEFINED");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_UNSUPPORTED");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
      LOGE("OpenGL Error(%s)\n", "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS");
      break;
    }

    return status != GL_FRAMEBUFFER_COMPLETE;
  }

  inline bool checkGLVersion(GLint MajorVersionRequire, GLint MinorVersionRequire)
  {
    GLint MajorVersionContext = 0;
    GLint MinorVersionContext = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &MajorVersionContext);
    glGetIntegerv(GL_MINOR_VERSION, &MinorVersionContext);
    return (MajorVersionContext * 100 +  MinorVersionContext * 10) 
      >= (MajorVersionRequire* 100 + MinorVersionRequire * 10);
  }

  inline bool checkExtension(char const * String)
  {
    GLint ExtensionCount = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &ExtensionCount);
    for(GLint i = 0; i < ExtensionCount; ++i)
      if(std::string((char const*)glGetStringi(GL_EXTENSIONS, i)) == std::string(String))
        return true;
    return false;
  }

  inline bool checkError(const char* Title)
  {
    int Error;
    if((Error = glGetError()) != GL_NO_ERROR)
    {
      std::string ErrorString;
      switch(Error)
      {
      case GL_INVALID_ENUM:
        ErrorString = "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        ErrorString = "GL_INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        ErrorString = "GL_INVALID_OPERATION";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        ErrorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
      case GL_OUT_OF_MEMORY:
        ErrorString = "GL_OUT_OF_MEMORY";
        break;
      default:
        ErrorString = "UNKNOWN";
        break;
      }
      LOGE("OpenGL Error(%s): %s\n", ErrorString.c_str(), Title);
    }
    return Error == GL_NO_ERROR;
  }

  bool checkBindings(int bindingBits, const char* marker, int num)
  {
    bool bound = false;
    GLint obj = 0;

#define GLERRCHECKBOUND( name, obj )   \
      glGetIntegerv(name, (obj)); \
      if(*(obj)) { \
        bound = true; \
        LOGW("OpenGL bound: %s\n", #name); \
      }

#define GLERRCHECKBOUNDFN( fn, what, obj )   \
      fn; \
      if(*(obj)) { \
        bound = true; \
        LOGW("OpenGL bound: %s\n", what); \
      }

#define GLERRCHECKBOUNDUNIT( i, name, obj )   \
      glGetIntegerv(name, (obj)); \
      if(*(obj)) { \
        bound = true; \
        LOGW("OpenGL bound: %s %d\n", #name, i); \
      }

#define GLERRCHECKBOUNDINDEXED(name, i, obj) \
    glGetIntegeri_v( name, i, (obj) ); \
    if (*(obj)) { \
      bound = true; \
      LOGW("OpenGL bound: %s %d\n", #name, i); \
    }

    if (bindingBits & CHECKBINDING_VAO_BIT){
      GLERRCHECKBOUND( GL_VERTEX_ARRAY_BINDING, &obj );
    }
    if (bindingBits & CHECKBINDING_FBO_BIT){
      GLERRCHECKBOUND( GL_DRAW_FRAMEBUFFER_BINDING, &obj );
      GLERRCHECKBOUND( GL_READ_FRAMEBUFFER_BINDING, &obj );
    }
    if (bindingBits & CHECKBINDING_PROGRAM_BIT){
      GLERRCHECKBOUND( GL_CURRENT_PROGRAM, &obj );
      GLERRCHECKBOUND( GL_PROGRAM_PIPELINE_BINDING, &obj );
    }
    if (bindingBits & CHECKBINDING_IMAGES_BIT){
      GLint units;
      glGetIntegerv(GL_MAX_IMAGE_UNITS, &units);
      for (int i = 0; i < units; i++){
        GLERRCHECKBOUNDINDEXED(GL_IMAGE_BINDING_NAME, i, &obj);
      }
    }
    if (bindingBits & CHECKBINDING_BUFFERS_BIT){
      GLERRCHECKBOUND(GL_ARRAY_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_ELEMENT_ARRAY_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_PIXEL_PACK_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_PIXEL_UNPACK_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_UNIFORM_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_TRANSFORM_FEEDBACK_BINDING, &obj);
      GLERRCHECKBOUND(GL_SHADER_STORAGE_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_ATOMIC_COUNTER_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_TEXTURE_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_COPY_READ_BUFFER_BINDING, &obj);
      GLERRCHECKBOUND(GL_COPY_WRITE_BUFFER_BINDING, &obj);

      GLint units;
      glGetIntegerv( GL_MAX_VERTEX_ATTRIB_BINDINGS, &units );
      for (int i = 0; i < units; i++){
        GLERRCHECKBOUNDINDEXED(GL_VERTEX_BINDING_BUFFER, i, &obj);
      }

      glGetIntegerv( GL_MAX_UNIFORM_BUFFER_BINDINGS, &units );
      for (int i = 0; i < units; i++){
        GLERRCHECKBOUNDINDEXED( GL_UNIFORM_BUFFER_BINDING, i, &obj );
      }

      glGetIntegerv( GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &units );
      for (int i = 0; i < units; i++){
        GLERRCHECKBOUNDINDEXED( GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, i, &obj );
      }

      glGetIntegerv( GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &units );
      for (int i = 0; i < units; i++){
        GLERRCHECKBOUNDINDEXED( GL_SHADER_STORAGE_BUFFER_BINDING, i, &obj );
      }

      glGetIntegerv( GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &units );
      for (int i = 0; i < units; i++){
        GLERRCHECKBOUNDINDEXED( GL_ATOMIC_COUNTER_BUFFER_BINDING, i, &obj );
      }
    }

    
    if (bindingBits & (CHECKBINDING_TEXTURES_BIT | CHECKBINDING_SAMPLERS_BIT)){
      GLint currentUnit;
      GLint units;

      glGetIntegerv(GL_ACTIVE_TEXTURE, &currentUnit);
      glGetIntegerv( GL_MAX_TEXTURE_IMAGE_UNITS, &units );

      for (int i = 0; i < units; i++){
        glActiveTexture( GL_TEXTURE0 + i );
        if (bindingBits & CHECKBINDING_TEXTURES_BIT){
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_1D, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_2D, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_3D, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_1D_ARRAY, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_2D_ARRAY, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_CUBE_MAP, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_2D_MULTISAMPLE, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_CUBE_MAP, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_CUBE_MAP_ARRAY, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_BUFFER, &obj );
          GLERRCHECKBOUNDUNIT( i, GL_TEXTURE_BINDING_RECTANGLE, &obj );
        }
        if (bindingBits & CHECKBINDING_SAMPLERS_BIT){
          GLERRCHECKBOUND( GL_SAMPLER_BINDING, &obj );
        }
      }

      glActiveTexture(currentUnit);
    }

    if (bindingBits & CHECKBINDING_XFB_BIT){
      GLERRCHECKBOUND( GL_TRANSFORM_FEEDBACK_BINDING, &obj );
    }

    if (bindingBits & CHECKBINDING_VATTRIBS_BIT){
      GLint units;
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &units);
      GLint enabled;
      for (int i = 0; i < units; i++){
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
        if (enabled){
          bound = true;
          LOGW("OpenGL enabled: vertex %d\n", i);
        }
      }
    }

    if (bound){
      if (marker){
        LOGW("%s %d\n\n",marker,num);
      }
      else{
        LOGW("\n");
      }
    }
#undef GLERRCHECKBOUNDUNIT
#undef GLERRCHECKBOUND
#undef GLERRCHECKBOUNDINDEXED
#undef GLERRCHECKBOUNDFN

    return !bound;
  }
}


#endif//NV_ERROR_INCLUDED
