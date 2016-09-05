using UnityEngine;
using System.Collections;

public class NPCBody : MonoBehaviour {

    NavMeshAgent gAgent;
    Animator gAnimator;

    private static float m_OrientationRatio = 180f;

    private Vector3 gForward;

    #region Properties
    public bool Navigation;
    #endregion

    [System.ComponentModel.DefaultValue(1f)]
    private float MaxWalkSpeed { get; set; }

    [System.ComponentModel.DefaultValue(2f)]
    private float MaxRunSpeed { get; set; }

    void Reset() {
        Debug.Log("Initializing NPCBody ... ");
        gAgent = GetComponent<NavMeshAgent>();
        gAnimator = GetComponent<Animator>();
        if (gAgent == null) {
            gAgent = gameObject.AddComponent<NavMeshAgent>();
            Debug.Log("NPCBody requires a NavMeshAgent if navigation is on, adding a default one.");
        }
        if (gAnimator == null) {
            throw new System.Exception("NPCBody requires an Animator !!! ");
        }
    }

    void Start() {
        gForward = transform.forward;
    }

    public void UpdateBody() {

    }

}
