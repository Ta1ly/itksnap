/*=========================================================================

  Program:   ITK-SNAP
  Module:    $RCSfile: Filename.cxx,v $
  Language:  C++
  Date:      $Date: 2010/10/18 11:25:44 $
  Version:   $Revision: 1.12 $
  Copyright (c) 2011 Paul A. Yushkevich

  This file is part of ITK-SNAP

  ITK-SNAP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

=========================================================================*/

#ifndef GENERICSLICERENDERER_H
#define GENERICSLICERENDERER_H

#include <AbstractRenderer.h>
#include <GenericSliceModel.h>
#include <ImageWrapper.h>
#include <OpenGLSliceTexture.h>
#include <SNAPOpenGL.h>
#include <list>
#include <LayerAssociation.h>

class GenericSliceRenderer;

class SliceRendererDelegate : public AbstractRenderer
{
public:
  virtual ~SliceRendererDelegate() {}

  irisGetSetMacro(ParentRenderer, GenericSliceRenderer *)

protected:
  GenericSliceRenderer *m_ParentRenderer;
};

struct OpenGLTextureAssociationFactory
{
  typedef OpenGLSliceTexture<ImageWrapperBase::DisplayPixelType> Texture;
  Texture *New(ImageWrapperBase *layer);
  GenericSliceRenderer *m_Renderer;
};

class GenericSliceRenderer : public AbstractRenderer
{
public:

  irisITKObjectMacro(GenericSliceRenderer, AbstractModel)

  FIRES(ModelUpdateEvent)

  void SetModel(GenericSliceModel *model);

  void initializeGL();
  void resizeGL(int w, int h);
  void paintGL();

  irisGetMacro(Model, GenericSliceModel *)
  irisIsMacro(ThumbnailDrawing)

  typedef std::list<SliceRendererDelegate *> RendererDelegateList;

  // Get a reference to the list of overlays stored in here
  const RendererDelegateList &GetOverlays() const
    { return m_Overlays; }

  RendererDelegateList &GetOverlays()
    { return m_Overlays; }

  // This method can be used by the renderer delegates to draw a texture
  void DrawTextureForLayer(ImageWrapperBase *layer, bool use_transparency);

  // A callback for when the model is reinitialized
  // void OnModelReinitialize();

protected:

  GenericSliceRenderer();
  virtual ~GenericSliceRenderer() {}

  void OnUpdate();

  void DrawMainTexture();
  void DrawSegmentationTexture();
  void DrawOverlayTexture();
  void DrawThumbnail();
  void DrawOverlays();

  // Draw the image and overlays either on top of each other or separately
  // in individual cells. Returns true if a layer was drawn, false if not,
  // i.e., the cell is outside of the range of available layers
  bool DrawImageLayers(int nrows, int ncols, int irow, int icol);

  GenericSliceModel *m_Model;

  // Whether rendering to thumbnail or not
  bool m_ThumbnailDrawing;

  // A dynamic association between various image layers and texture objects
  typedef OpenGLSliceTexture<ImageWrapperBase::DisplayPixelType> Texture;
  typedef LayerAssociation<Texture, ImageWrapperBase,
                           OpenGLTextureAssociationFactory> TextureMap;

  TextureMap m_Texture;

  // A list of overlays that the user can configure
  RendererDelegateList m_Overlays;

  // Internal method used by UpdateTextureMap()
  // void AssociateTexture(ImageWrapperBase *iw, TextureMap &src, TextureMap &trg);

  // Texture factory method
  Texture *CreateTexture(ImageWrapperBase *iw);


  // Update the texture map to mirror the current images in the model
  void UpdateTextureMap();

  friend struct OpenGLTextureAssociationFactory;
};



#endif // GENERICSLICERENDERER_H
