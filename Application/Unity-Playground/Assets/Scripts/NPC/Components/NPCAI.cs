using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Reflection;

namespace NPC {

    [System.Serializable]
    public class NPCAI {

        public NPCAI(NPCController pController) {
            this.gNPCController = pController;
        }

        #region NPC_Modules
        private NPCController gNPCController;
        #endregion

        #region NPC_Goals
        private Stack<NPCGoal> gGoals;
        #endregion

        #region Traits

        private Dictionary<string, NPCAttribute> gAttributes;

        /* For the purpose of initialization */
        private bool gRandomizeTraits;

        bool RandomizeTraits {

            get {
                return gRandomizeTraits;
            }
            set {
                gRandomizeTraits = value;
            }
        }

        #endregion

        protected void InitializeTraits() {
            foreach (PropertyInfo pi in this.GetType().GetProperties()) {
                object[] attribs = pi.GetCustomAttributes(true);
                if(attribs.Length > 0) {

                }
            }
        }

        #region Attributes
    
        [NPCAttribute("NPC",typeof(bool))]
        public bool NPC { get; set; }

        [NPCAttribute("Charisma",typeof(float))]
        public float Charisma { get; set; }

        [NPCAttribute("Friendliness",typeof(float))]
        public float Friendliness { get; set; }
    
        [NPCAttribute("Strength",typeof(int))]
        public int Strength { get; set; }

        [NPCAttribute("Intelligence",typeof(int))]
        public int Intelligence { get; set; }

        [NPCAttribute("Dexterity",typeof(int))]
        public int Dexterity { get; set; }

        [NPCAttribute("Constitution",typeof(int))]
        public int Constitution { get; set; }

        [NPCAttribute("Hostility", typeof(float))]
        public float Hostility { get; set; }

        [NPCAttribute("Location",typeof(Vector3))]
        public Vector3 Location { get; set; }

        #endregion

    }
}