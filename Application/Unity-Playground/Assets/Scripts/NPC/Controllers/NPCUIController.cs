using UnityEngine;
using UnityEngine.UI;
using System.Collections;

namespace NPC {

    public class NPCUIController : MonoBehaviour {

        public Text CameraMode;
        public NPCCamController NPCCamera;
        public bool Debug = true;

        // Update is called once per frame
        public void UpdateUI() {
            if (Debug) {
                if(NPCCamera != null) {
                    CameraInfo();
                }
            }
        }

        /// <summary>
        /// Displays Camera Data
        /// </summary>
        void CameraInfo() {
            CameraMode.text = "Camera Mode: ";
            switch (NPCCamera.CurrentMode) {
                case NPCCamController.CAMERA_MODE.FIRST_PERSON:
                    CameraMode.text += "First Person";
                    break;
                case NPCCamController.CAMERA_MODE.THIRD_PERSON:
                    CameraMode.text += "Third Person";
                    break;
                case NPCCamController.CAMERA_MODE.FREE:
                    CameraMode.text += "Free Flight";
                    break;
            }
        }
    }

}
