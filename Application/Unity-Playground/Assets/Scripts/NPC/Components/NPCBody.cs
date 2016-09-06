using UnityEngine;
using System.Collections;

public class NPCBody : MonoBehaviour {

    NavMeshAgent gAgent;
    Rigidbody gRigidBody;
    Animator gAnimator;
    CapsuleCollider gCapsuleCollider;

    private static float m_OrientationRatio = 180f;

    private Vector3 gForward;

    #region Properties
    public bool Navigation;
    #endregion

    [System.ComponentModel.DefaultValue(1f)]
    private float MaxWalkSpeed { get; set; }

    [System.ComponentModel.DefaultValue(2f)]
    private float MaxRunSpeed { get; set; }

    [System.ComponentModel.DefaultValue(-1f)]
    private float TurnLeftAngle { get; set; }

    [System.ComponentModel.DefaultValue(1f)]
    private float TurnRightAngle { get; set; }

    void Reset() {
        Debug.Log("Initializing NPCBody ... ");
        gAgent = gameObject.GetComponent<NavMeshAgent>();
        gRigidBody = gameObject.GetComponent<Rigidbody>();
        gAnimator = gameObject.GetComponent<Animator>();
        gCapsuleCollider = gameObject.GetComponent<CapsuleCollider>();
        if (gAgent == null) {
            gAgent = gameObject.AddComponent<NavMeshAgent>();
            gAgent.height = 0.5f;
            gAgent.autoBraking = false;
            gAgent.enabled = false;
            Debug.Log("NPCBody requires a NavMeshAgent if navigation is on, adding a default one.");
        }
        if (gAnimator.runtimeAnimatorController == null) {
            Debug.Log("NPCBody requires an Animator Controller!!! ");
            // Load the default NPC Animator controller
            gAnimator.runtimeAnimatorController = (RuntimeAnimatorController)Resources.Load("NPCResources/Animators/NPCDefaultAnimatorController");
        }
        if(gRigidBody == null) {
            gRigidBody = gameObject.AddComponent<Rigidbody>();
            gRigidBody.useGravity = true;
            gRigidBody.mass = 3;
            gRigidBody.constraints = RigidbodyConstraints.FreezeRotation;
        }
        if(gCapsuleCollider == null) {
            gCapsuleCollider = gameObject.AddComponent<CapsuleCollider>();
            gCapsuleCollider.radius = 0.3f;
            gCapsuleCollider.height = 1.5f;
            gCapsuleCollider.center = new Vector3(0.0f,0.75f,0.0f);
        }
    }

    public void UpdateBody() { }

}
