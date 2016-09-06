using UnityEngine;
using System.Collections;

public class NPCController : MonoBehaviour, IPerceivable {
    
    #region AI
    private NPCAI gAI;
    #endregion

    #region BODY
    private NPCBody gBody;
    #endregion

    #region PERCEPTION
    private NPCPerception gPerception;
    #endregion

    #region Properties
    public NPCPerception Perception {
        get { return gPerception; }
    }

    public NPCBody Body {
        get { return gBody; }
    }
    #endregion

    #region Unity_Runtime

    // Use this for initialization
    void Start () {
        gBody = gameObject.GetComponent<NPCBody>();
        gPerception = gameObject.GetComponent<NPCPerception>();
    }
	
    void FixedUpdate() {
        // update IO here
    }

	// Update is called once per frame
	void Update () { }

    // When script is added to GameObject or Reset
    void Reset() {
        Debug.Log("Resetting NPCController");
        if (GetComponent<NPCBody>() != null) DestroyImmediate(GetComponent<NPCBody>());
        InitializeNPCComponents();
        // InitializePerceptionField();
    }

    #endregion

    #region Initialization

    private void InitializeNPCComponents() {
        gAI = new NPCAI(this);
        gPerception = gameObject.AddComponent<NPCPerception>();
        gBody = gameObject.AddComponent<NPCBody>();
        // hide flags
        // gBody.hideFlags = HideFlags.HideInInspector;
    }

    #endregion
}
