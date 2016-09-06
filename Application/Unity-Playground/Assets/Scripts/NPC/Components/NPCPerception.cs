using UnityEngine;
using System.Collections;

public class NPCPerception : MonoBehaviour {

    /// <summary>
    /// Main Controller Handle
    /// </summary>
    NPCController g_Controller;
    private static string PERCEPTION_LAYER = "PerpcetionField";

    #region Static Fields
    public static float MIN_VIEW_ANGLE = 75f;
    public static float MAX_VIEW_ANGLE = 180f;
    public static float MIN_PERCEPTION_FIELD = 4f;
    public static float MAX_PERCEPTION_FIELD = 20f;
    #endregion

    #region Perception
    [SerializeField]
    private SphereCollider gPerceptionField;

    [SerializeField]
    private float gViewAngle = 135f;
    #endregion

    #region Properties
    
    public float ViewAngle {
        get { return this.gViewAngle; }
        set { this.gViewAngle = value; }
    }

    
    public float PerceptionRadius {
        get { return gPerceptionField.radius; }
        set { this.gPerceptionField.radius = value; }
    }

    
    public SphereCollider PerceptionField {
        get { return this.gPerceptionField; }
        set { gPerceptionField = value; }
    }   
    #endregion

    void Reset() {
        Debug.Log("Initializing NPCPerception...");
        // add perception fields
        g_Controller = gameObject.GetComponent<NPCController>();
        GameObject pf = new GameObject();
        pf.name = "Perception Field";
        pf.layer = LayerMask.NameToLayer(PERCEPTION_LAYER);
        // set the perception layer
        pf.transform.parent = g_Controller.transform;
        gPerceptionField = pf.AddComponent<SphereCollider>();
        gPerceptionField.isTrigger = true;
        gPerceptionField.radius = (MIN_PERCEPTION_FIELD + MAX_PERCEPTION_FIELD) / 2;
        gViewAngle = (MIN_VIEW_ANGLE + MAX_VIEW_ANGLE) / 2;
        // collisions / reach
    }

    public void UpdatePerception() { }

}