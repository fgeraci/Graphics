using UnityEngine;
using System.Collections;


namespace NPC {

    [RequireComponent(typeof(Animator))]
    [System.Serializable]
    public class NPCIKController : MonoBehaviour {

     //   Animator gAnimator;

     //   /* Test Targets */
     //   public Transform IK_TARGET_LEFT_FOOT;
     //   public Transform IK_TARGET_RIGHT_FOOT;
     //   public Transform LOOK_AT_TARGET;
     //   public Transform GRAB_RIGHT_TARGET;

     //   /* Hints */
     //   public Transform HINT_LEFT_KNEE;
     //   public Transform HINT_RIGHT_KNEE;

     //   /* Weights*/
     //   public float IK_WEIGHT;

     //   /* Enable disabled IK and COmponents during runtime */
     //   public bool IK_ACTIVE;
     //   public bool USE_HINTS;
    
    

     //   /* Bones */
     //   Transform HEAD;
     //   Transform RIGHT_HAND;
     //   Transform LEFT_HAND;
     //   Transform RIGHT_FOOT;
     //   Transform LEFT_FOOT;
    
    
     //   // Use this for initialization
     //   void Start () {

     //       gAnimator = GetComponent<Animator>();
        
     //       // Find Bones
     //       RIGHT_HAND = gAnimator.GetBoneTransform(HumanBodyBones.RightHand);
     //       LEFT_HAND = gAnimator.GetBoneTransform(HumanBodyBones.LeftHand);
     //       RIGHT_FOOT = gAnimator.GetBoneTransform(HumanBodyBones.RightFoot);
     //       LEFT_FOOT = gAnimator.GetBoneTransform(HumanBodyBones.LeftFoot);
     //       HEAD = gAnimator.GetBoneTransform(HumanBodyBones.Head);

     //       // default weight
     //       IK_WEIGHT = IK_WEIGHT < 0.1f ?  1f : IK_WEIGHT;

     //       Debug.Log(gAnimator == null ? "NPCIK - Animator missing ... " : "");
     //   }
	
     //   // main IK method called every frame
	    //void OnAnimatorIK() {
     //       if(gAnimator) {
     //           if(IK_ACTIVE) {
                
     //               /* Feet */
     //               DoFeetIK();

     //               /* Look At */
     //               gAnimator.SetLookAtPosition(LOOK_AT_TARGET.position);
     //               gAnimator.SetLookAtWeight(1f);
     //           }
     //       }
     //   }

     //   private void DoFeetIK() {
     //       // set: Weights, Positions and Hints (of joints) and Rorations

     //           // Main weights
     //       gAnimator.SetIKPositionWeight(AvatarIKGoal.LeftFoot, IK_WEIGHT);
     //       gAnimator.SetIKPositionWeight(AvatarIKGoal.RightFoot, IK_WEIGHT);
     //           // position
     //       gAnimator.SetIKPosition(AvatarIKGoal.LeftFoot, IK_TARGET_LEFT_FOOT.position);
     //       gAnimator.SetIKPosition(AvatarIKGoal.RightFoot, IK_TARGET_RIGHT_FOOT.position);
     //           // hints
     //       if (USE_HINTS) {
     //           gAnimator.SetIKHintPositionWeight(AvatarIKHint.LeftKnee, IK_WEIGHT);
     //           gAnimator.SetIKHintPositionWeight(AvatarIKHint.RightKnee, IK_WEIGHT);
     //           gAnimator.SetIKHintPosition(AvatarIKHint.LeftKnee, HINT_LEFT_KNEE.position);
     //           gAnimator.SetIKHintPosition(AvatarIKHint.RightKnee, HINT_RIGHT_KNEE.position);
     //       }
     //           // rotation
     //       gAnimator.SetIKRotationWeight(AvatarIKGoal.LeftFoot, IK_WEIGHT);
     //       gAnimator.SetIKRotationWeight(AvatarIKGoal.RightFoot, IK_WEIGHT);
     //       gAnimator.SetIKRotation(AvatarIKGoal.LeftFoot, IK_TARGET_LEFT_FOOT.rotation);
     //       gAnimator.SetIKRotation(AvatarIKGoal.RightFoot, IK_TARGET_RIGHT_FOOT.rotation);
     //   }

     //   Transform LOOK_AT {
     //       get {
     //           return this.LOOK_AT_TARGET;
     //       }
     //       set {
     //           this.LOOK_AT_TARGET = value;
     //       }
     //   }

     //   Transform GRAB_RIGHT {
     //       get {
     //           return this.GRAB_RIGHT_TARGET;
     //       }
     //       set {
     //           this.GRAB_RIGHT_TARGET = value;
     //       }
     //   }
    }

}
