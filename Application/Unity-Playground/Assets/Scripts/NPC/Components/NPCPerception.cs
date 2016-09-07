using UnityEngine;
using System.Collections;

namespace NPC {

    [System.Serializable]
    public class NPCPerception : MonoBehaviour {

        /// <summary>
        /// Main Controller Handle
        /// </summary>
        NPCController g_Controller;
        private static string PERCEPTION_LAYER = "PerpcetionField";
        private static string PERCEPTION_FIELD_OBJECT = "PerpcetionField";

        #region Static Fields
        public static float MIN_VIEW_ANGLE = 75f;
        public static float MAX_VIEW_ANGLE = 180f;
        public static float MIN_PERCEPTION_FIELD = 2f;
        public static float MAX_PERCEPTION_FIELD = 10f;
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
            GameObject pf;
            Component sCol = g_Controller.GetComponent(PERCEPTION_FIELD_OBJECT);
            if (sCol == null) {
                // take into account not readding a duplicate Sphere Collider in the same layer
                pf = new GameObject();
            } else pf = sCol.gameObject;
            pf.name = PERCEPTION_FIELD_OBJECT;
            pf.layer = LayerMask.NameToLayer(PERCEPTION_LAYER);
            pf.transform.parent = g_Controller.transform;
            gPerceptionField = pf.AddComponent<SphereCollider>();
            gPerceptionField.isTrigger = true;
            gPerceptionField.radius = (MIN_PERCEPTION_FIELD + MAX_PERCEPTION_FIELD) / 4;
            gViewAngle = (MIN_VIEW_ANGLE + MAX_VIEW_ANGLE) / 2;
            // collisions / reach
        }

        public void UpdatePerception() { }

    }

}