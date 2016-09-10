using UnityEngine;
using System.Collections;

namespace NPC {

    public enum LOCO_STATE {
        IDLE,
        FRONT,
        FORWARD,
        BACKWARDS,
        LEFT,
        RIGHT,
        RUN,
        WALK,
        DUCK,
        GROUND,
        JUMP
    }

    [System.Serializable]
    public class NPCBody : MonoBehaviour {

        NavMeshAgent gAgent;
        Rigidbody gRigidBody;
        Animator g_Animator;
        CapsuleCollider gCapsuleCollider;

        private static string g_AnimParamSpeed      = "Speed";
        private static string g_AnimParamDirection  = "Direction";
        private static string g_AnimParamJump       = "Jump";

        private static int SPEED_MOD            =  2;
        private static float MAX_WALK__SPEED    =  1.00f;
        private static float MAX_RUN_SPEED      =  1.00f * SPEED_MOD;
        private static float MIN_WALK_SPEED     =  -1* MAX_WALK__SPEED;
        private static float MIN_RUN_SPEED      =  -1 * MAX_WALK__SPEED;

        private LOCO_STATE g_CurrentStateFwd    = LOCO_STATE.IDLE;
        private LOCO_STATE g_CurrentStateGnd    = LOCO_STATE.GROUND;
        private LOCO_STATE g_CurrentStateDir    = LOCO_STATE.FRONT;
        private LOCO_STATE g_CurrentStateMod    = LOCO_STATE.WALK;

        // This correlate with the parameters from the Animator
        private float g_CurrentSpeed            = 0.0f;
        private float g_CurrentVelocity         = 0.05f;
        private float g_CurrentOrientation      = 0.0f;
        
        #region Properties
        public bool Navigation;

        public bool IsIdle {
            get {
                return (g_CurrentStateFwd == LOCO_STATE.IDLE) 
                    && (g_CurrentStateDir == LOCO_STATE.FRONT
                    && g_CurrentSpeed == 0.0f
                    && g_CurrentOrientation == 0.0f);
            }
        }
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
            g_Animator = gameObject.GetComponent<Animator>();
            gCapsuleCollider = gameObject.GetComponent<CapsuleCollider>();
            if (gAgent == null) {
                gAgent = gameObject.AddComponent<NavMeshAgent>();
                gAgent.height = 0.5f;
                gAgent.autoBraking = false;
                gAgent.enabled = false;
                Debug.Log("NPCBody requires a NavMeshAgent if navigation is on, adding a default one.");
            }
            if (g_Animator.runtimeAnimatorController == null) {
                enabled = false;
                throw new System.Exception("NPCBody --> Agent requires an Animator Controller!!! - consider addinnng thr NPCDefaultAnimatorController");
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

        void Start() {
            g_Animator = GetComponent<Animator>();
        }

        /// <summary>
        /// Control all the body's parameters for speed, orientation, etc...
        /// </summary>
        public void UpdateBody() {
            
            // handle mod
            float forth   = g_CurrentStateFwd == LOCO_STATE.FORWARD ? 1.0f : -1.0f;
            float orient  = g_CurrentStateDir == LOCO_STATE.RIGHT ? 1.0f : -1.0f;
            bool duck   = (g_CurrentStateMod == LOCO_STATE.DUCK);
            float topF = (g_CurrentStateMod == LOCO_STATE.RUN || g_CurrentSpeed > MAX_WALK__SPEED) 
                ? MAX_RUN_SPEED : MAX_WALK__SPEED;

            // update forward
            if (g_CurrentStateFwd != LOCO_STATE.IDLE) {
                if (g_CurrentSpeed > MAX_WALK__SPEED
                    && g_CurrentStateMod == LOCO_STATE.WALK) g_CurrentSpeed -= g_CurrentVelocity;
                else g_CurrentSpeed = Mathf.Clamp(g_CurrentSpeed + (g_CurrentVelocity * forth), MIN_WALK_SPEED, topF);
            } else {
                if(g_CurrentSpeed != 0.0f) {
                    float m = g_CurrentVelocity * (g_CurrentSpeed > 0.0f ? -1.0f : 1.0f);
                    float stopDelta = g_CurrentSpeed + m;
                    g_CurrentSpeed = Mathf.Abs(stopDelta) > 0.05f ? stopDelta : 0.0f;
                }
            }

            // update direction
            if (g_CurrentStateDir != LOCO_STATE.FRONT) {
                g_CurrentOrientation = Mathf.Clamp(g_CurrentOrientation + (g_CurrentVelocity * orient), -1.0f, 1.0f);
            } else {
                if (g_CurrentStateDir != 0.0f) {
                    float m = g_CurrentVelocity * (g_CurrentOrientation > 0.0f ? -1.0f : 1.0f);
                    float stopDelta = g_CurrentOrientation + m;
                    g_CurrentOrientation = Mathf.Abs(stopDelta) > 0.05f ? stopDelta : 0.0f;
                }
            }

            // update ground
            if(g_CurrentStateGnd == LOCO_STATE.JUMP) {
                g_Animator.SetTrigger(g_AnimParamJump);
                g_CurrentStateGnd = LOCO_STATE.GROUND;
            }

            // set animator
            g_Animator.SetFloat(g_AnimParamSpeed, g_CurrentSpeed);
            g_Animator.SetFloat(g_AnimParamDirection, g_CurrentOrientation);
        }

        #region Affordances
        
        public void Move(LOCO_STATE s) {
            switch (s) {
                case LOCO_STATE.RUN:
                case LOCO_STATE.DUCK:
                case LOCO_STATE.WALK:
                    g_CurrentStateMod = s;
                    break;
                case LOCO_STATE.FORWARD:
                case LOCO_STATE.BACKWARDS:
                case LOCO_STATE.IDLE:
                    g_CurrentStateFwd = s;
                    break;
                case LOCO_STATE.RIGHT:
                case LOCO_STATE.LEFT:
                case LOCO_STATE.FRONT:
                    g_CurrentStateDir = s;
                    break;
                case LOCO_STATE.JUMP:
                    g_CurrentStateGnd = s;
                    break;
                default:
                    Debug.Log("NPCBody --> Invalid direction especified for ModifyMotion");
                    break;
            }
        }
        
        #endregion
    }

}
