#ifndef ABSTRACTLAYERASSOCIATEDMODEL_H
#define ABSTRACTLAYERASSOCIATEDMODEL_H

#include "AbstractModel.h"
#include "LayerAssociation.h"
#include "GlobalUIModel.h"
#include "IRISException.h"
#include "IRISApplication.h"

/**
  This is an abstract class for a special type of UI model that can be
  associated with different image layers in SNAP. Examples of these models
  are models for contrast adjustment, colormap adjustment, etc., i.e.,
  models that link the GUI with one image layer. Two options were available:
  to associate a single such model with each image layer, or to create just
  one model, and allow the layer for that model to be switched out. I chose
  the second option because it reduced the number of models that have to be
  kept around. However, the model has to keep track of layer-specific
  properties, and also has to be smart about registering and unregistering
  for events originating from the layers.

  This class is templated over a properties object, which contains some
  layer-specific properties that the model needs to store. For example, it
  can be the number of histogram bins to display for the contrast dialog.

  The second parameter is the type of the image wrapper that can participate
  in the association. It can be ImageWrapperBase or one of its subclasses.
*/
template <class TProperties, class TWrapper = ImageWrapperBase>
class AbstractLayerAssociatedModel : public AbstractModel
{
public:

  typedef AbstractLayerAssociatedModel<TProperties,TWrapper> Self;
  typedef AbstractModel Superclass;
  typedef SmartPtr<Self> Pointer;
  typedef SmartPtr<const Self> ConstPointer;
  itkTypeMacro(AbstractLayerAssociatedModel, AbstractModel)

  // An event fired when the selected layer changes
  itkEventMacro(ActiveLayerChangedEvent, ModelUpdateEvent)
  itkEventMacro(LayerStructureChangedEvent, ModelUpdateEvent)

  FIRES(LayerStructureChangedEvent)
  FIRES(ActiveLayerChangedEvent)

  irisGetMacro(ParentModel, GlobalUIModel *)
  void SetParentModel(GlobalUIModel *parent)
  {
    // Store the parent model
    m_ParentModel = parent;

    // Associate the layers with properties.
    m_LayerProperties.SetImageData(
          m_ParentModel->GetDriver()->GetCurrentImageData());

    // Layer changes in the parent are rebroadcast as model updates
    Rebroadcast(m_ParentModel, LayerChangeEvent(), LayerStructureChangedEvent());

    // Set active layer to NULL
    this->SetLayer(NULL);
  }



  /**
    Set the layer with which the model is associated. This can be NULL,
    in which case, the model will be dissasociated from all layers.
    */
  void SetLayer(TWrapper *layer)
  {
    // Make sure the layer-specific stuff is up to date
    m_LayerProperties.Update();

    // Unregister from the current layer
    if(m_LayerProperties.find(m_Layer) != m_LayerProperties.end())
      this->UnRegisterFromLayer(m_Layer);

    // Set the layer
    m_Layer = layer;

    // Handle events. Need to be careful here, because layers are dynamically
    // changing, and we don't want to add more than one observer to any layer.
    // Note that we don't remove the observer from the old layer because when
    // this method is called, the old layer may have already been destroyed!
    if(m_Layer)
      this->RegisterWithLayer(m_Layer);

    // Fire an event to indicate the change
    InvokeEvent(ActiveLayerChangedEvent());
  }


  /** Get the layer associated with the model, or NULL if there is none */
  irisGetMacro(Layer, TWrapper *)

  /** Get the properties associated with the current layer */
  TProperties &GetProperties()
  {
    assert(m_Layer);
    TProperties *p = m_LayerProperties[m_Layer];
    return *p;
  }


  /**
    This method should be implemented by the child class. It registers
    the child class to rebroadcast whatever events it needs from the layer
    with which it has been associated to the downstream objects.
  */
  virtual void RegisterWithLayer(TWrapper *layer) = 0;


  /**
    This method should be implemented by the child class. It disconnects
    the chold class from the associated layer (just before breaking the
    association). For the Register/Unregister pair to work, the Register
    method implementation should retain the tag returned by the call to
    the Rebroadcast method. This tag can be placed in the layer-associated
    properties, and then used during the call to UnRegister.
. */
  virtual void UnRegisterFromLayer(TWrapper *layer) = 0;

  /**
    The model has its own OnUpdate implementation, which handles changes
    in the layer structure. If the event bucket has a LayerChangeEvent,
    the model will automatically rebuild it's layer associations, and
    may reset the current layer to NULL if the current layer has been
    removed.

    If child models reimplement OnUpdate(), they must call
    AbstractLayerAssociatedModel::OnUpdate() within the reimplemented method.
    */
  virtual void OnUpdate()
  {
    if(m_EventBucket->HasEvent(LayerChangeEvent()))
      {
      // If the layers have changed, we need to update the layer properties
      // object. Then we need to see if the current layer has actually been
      // destroyed
      m_LayerProperties.SetImageData(m_ParentModel->GetDriver()->GetCurrentImageData());
      m_LayerProperties.Update();

      // Was the current layer removed?
      if(m_LayerProperties.find(m_Layer) == m_LayerProperties.end())
        this->SetLayer(NULL);
      }
  }





protected:
  AbstractLayerAssociatedModel()
  {
    // Set up the factory
    PropertiesFactory factory;
    factory.m_Model = this;
    m_LayerProperties.SetDelegate(factory);

    m_ParentModel = NULL;
    m_Layer = NULL;
  }

  virtual ~AbstractLayerAssociatedModel() {}

  /** Create a  property object for a new layer */
  TProperties *CreateProperty(TWrapper *w)
  {
    return new TProperties();
  }

  // Parent model
  GlobalUIModel *m_ParentModel;

  // Currently associated layer
  TWrapper *m_Layer;

  // A factory class for creating properties
  class PropertiesFactory
  {
  public:
    TProperties *New(TWrapper *w)
      { return m_Model->CreateProperty(w); }
    Self *m_Model;
  };

  // Association between a layer and a set of properties
  typedef LayerAssociation<
    TProperties,TWrapper,PropertiesFactory> LayerMapType;

  LayerMapType m_LayerProperties;

};

#endif // ABSTRACTLAYERASSOCIATEDMODEL_H