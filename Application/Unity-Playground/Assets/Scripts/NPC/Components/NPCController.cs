using UnityEngine;
using System.Collections;

namespace NPC {

    public class NPCController : MonoBehaviour, IPerceivable {

        #region AI
        [SerializeField]
        private NPCAI gAI;
        #endregion

        #region BODY
        [SerializeField]
        private NPCBody gBody;
        #endregion

        #region PERCEPTION
        [SerializeField]
        private NPCPerception gPerception;
        #endregion

        [SerializeField]
        private bool gMainAgent = false;

        [SerializeField]
        private bool gInitialized = false;

        #region Properties
        [SerializeField]
        public NPCPerception Perception {
            get { return gPerception; }
        }

        [SerializeField]
        public NPCBody Body {
            get { return gBody; }
        }

        [SerializeField]
        public bool MainAgent {
            get { return gMainAgent; }
            set { gMainAgent = value; }
        }
        #endregion

        #region Unity_Runtime

        void Awake() {
            if(MainAgent) {
                FindObjectOfType<NPCControlManager>().SetTarget(this);
            }
        }

        // Use this for initialization
        void Start () {
            gBody = gameObject.GetComponent<NPCBody>();
            gPerception = gameObject.GetComponent<NPCPerception>();
        }
	
        void FixedUpdate() {
            gBody.UpdateBody();
        }

	    // Update is called once per frame
	    void Update () { }

        // When script is added to GameObject or Reset
        void Reset() {
            if(!gInitialized) {
                Debug.Log("Creating NPCController");
                gMainAgent = false;
                if (GetComponent<NPCBody>() != null) DestroyImmediate(GetComponent<NPCBody>());
                if (GetComponent<NPCPerception>() != null) DestroyImmediate(GetComponent<NPCPerception>());
                InitializeNPCComponents();
                gInitialized = true;
            } else {
                Debug.Log("Loading existing NPCController settings");
            }
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

}
