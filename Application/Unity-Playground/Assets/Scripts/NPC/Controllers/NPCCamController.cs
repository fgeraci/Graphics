using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;

namespace NPC {

    public class NPCCamController : MonoBehaviour {
    
        public float Speed = 1.0f;
        private float gMouseX;
        private float gMouseY;
        public float CameraRotationSpeed = 20f;
    
        Transform Camera = null;
        private NPCController Target = null;

        public bool Targetting {
            get {
                return CurrentMode == CAMERA_MODE.THIRD_PERSON ||
                    CurrentMode == CAMERA_MODE.FIRST_PERSON || 
                    CurrentMode == CAMERA_MODE.ISOMETRIC_FOLLOW;
            }
        }
        
        public CAMERA_MODE CurrentMode;

        public enum CAMERA_MODE {
            FREE,
            THIRD_PERSON,
            FIRST_PERSON,
            ISOMETRIC,
            ISOMETRIC_FOLLOW
        }
        
        public void SetCamera(Transform t) {
            Camera = t;
        }

        public void SetTarget(NPCController t) {
            Target = t;
        }

        void Start() {
            if(Target != null) {
                SetThirdPersonView();
            }
        }

        public void UpdateCamera() {
            switch (CurrentMode) {
                case CAMERA_MODE.FREE:
                    HandleFreeCamera();
                    break;
                case CAMERA_MODE.FIRST_PERSON:
                    if (!Target.Body.IsIdle)
                        SetFirstPersonView();
                    break;
                case CAMERA_MODE.THIRD_PERSON:
                    if(!Target.Body.IsIdle)
                        SetThirdPersonView();
                    break;
                case CAMERA_MODE.ISOMETRIC:
                    HandleIsometricCamera();
                    break;
            }
        }
     
        public void UpdateCameraMode(CAMERA_MODE mode) {
            bool noTarget = false;
            CurrentMode = mode;
            switch (CurrentMode) {
                case CAMERA_MODE.FREE:
                    if (Target != null) SetThirdPersonView();
                    break;
                case CAMERA_MODE.FIRST_PERSON:
                    if (Target != null) SetFirstPersonView();
                    else noTarget = true;
                    break;
                case CAMERA_MODE.THIRD_PERSON:
                    if (Target != null) SetThirdPersonView();
                    else noTarget = true;
                    break;
                case CAMERA_MODE.ISOMETRIC:
                    SetIsometricView();
                    break;
            }
            if(noTarget) {
                CurrentMode = CAMERA_MODE.FREE;
                Debug.Log("NPCCamControlelr --> No target agent set, camera stays in FREE mode.");
            }
        }

        private void HandleCameraRotation() {
            if(Input.GetMouseButton(1)) {
                float mouseYRot = (Input.mousePosition.x - gMouseX) * Time.deltaTime * CameraRotationSpeed;
                float mouseXRot = (Input.mousePosition.y - gMouseY) * Time.deltaTime * CameraRotationSpeed;
                // we set the rotation as current angles + delta
                transform.eulerAngles = new Vector3(-mouseXRot + transform.localEulerAngles.x, 
                    transform.localEulerAngles.y + mouseYRot , 0);
            }
        }

        private void HandleFreeCamera() {
            float speedModifier = Input.GetKey(KeyCode.LeftShift) ? Speed * 2f : Speed;
            HandleCameraRotation();
            if (Input.GetKey(KeyCode.W)) {
                transform.position += transform.forward * (Time.deltaTime * speedModifier);
            } else if (Input.GetKey(KeyCode.S)) {
                transform.position -= transform.forward * (Time.deltaTime * speedModifier);
            }
            if (Input.GetKey(KeyCode.A)) {
                transform.position -= transform.right * (Time.deltaTime * speedModifier);
            } else if (Input.GetKey(KeyCode.D)) {
                transform.position += transform.right * (Time.deltaTime * speedModifier);
            }
            gMouseX = Input.mousePosition.x;
            gMouseY = Input.mousePosition.y;
        }

        private void HandleIsometricCamera() {
            float speedModifier = Input.GetKey(KeyCode.LeftShift) ? Speed * 2f : Speed;
            if (Input.GetKey(KeyCode.W)) {
                Camera.position += Vector3.right * (Time.deltaTime * speedModifier);
            } else if (Input.GetKey(KeyCode.S)) {
                Camera.position -= Vector3.right * (Time.deltaTime * speedModifier);
            }
            if (Input.GetKey(KeyCode.A)) {
                Camera.position += Vector3.forward * (Time.deltaTime * speedModifier);
            } else if (Input.GetKey(KeyCode.D)) {
                Camera.position -= Vector3.forward * (Time.deltaTime * speedModifier);
            }

            Debug.Log(Input.GetAxis("Mouse ScrollWheel"));
            if(Input.GetAxis("Mouse ScrollWheel") > 0.0f) {
                Camera.position -= Vector3.up * (0.08f * speedModifier);
            } else if (Input.GetAxis("Mouse ScrollWheel") < 0.0f) {
                Camera.position += Vector3.up * (0.08f * speedModifier);
            }
        }

        private void SetThirdPersonView() {
            Camera.position = Target.transform.position;
            Camera.rotation = Target.transform.rotation;
            Camera.position += Camera.up * 0.8f;
            Camera.position += Camera.forward * -0.6f;
            Camera.position += Camera.right * 0.2f;
            Camera.RotateAround(Camera.position, Camera.right, 15f);
        }

        private void SetFirstPersonView() {
            Camera.position = Target.transform.position;
            Camera.rotation = Target.transform.rotation;
            Camera.position += Target.transform.forward * 0.1f;
            Camera.position += Target.transform.up * 0.45f;
        }

        private void SetIsometricView() {
            Vector3 curPos = Camera.position;
            Camera.rotation = Quaternion.identity;
            Camera.Rotate(Vector3.up, 90.0f);
            Camera.Rotate(Vector3.right, 35.0f);
            Debug.Log(Camera.position);
            Camera.position = new Vector3(curPos.x, 2.0f, curPos.z);
            Camera.position -= (Vector3.right * 0.5f);
        }
    }

}
