﻿using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace NPCModules {

    public enum NPCGOAL_TYPE {
        BODY,
        TRAIT,
        STATE
    }

    public enum NPCGOAL_STATUS {
        PENDING,
        SATISFIED,
        INTERRUPTED,
        FAILED
    }

    public class NPCGoal {

        private NPCGOAL_TYPE gType;
        private NPCGOAL_STATUS gStatus;
        private Dictionary<string, NPCAttribute> gAttributes;

        #region Properties
        public NPCGOAL_STATUS Status {
            get {
                return this.gStatus;
            }
        }

        public NPCGOAL_TYPE Type {
            get {
                return this.gType;
            }
        }



        #endregion
    }

}