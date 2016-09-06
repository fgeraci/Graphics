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
        private Transform Target = null;

        Vector3 GlobalOffset;
        public CAMERA_MODE CurrentMode;

        public enum CAMERA_MODE {
            FREE,
            THIRD_PERSON,
            FIRST_PERSON
        }

        public void SetCamera(Transform t) {
            Camera = t;
        }

        public void SetTarget(Transform t) {
            Target = t;
        }

        public void UpdateCamera() {
            switch (CurrentMode) {
                case CAMERA_MODE.FREE:
                    HandleFreeCamera();
                    break;
                case CAMERA_MODE.FIRST_PERSON:
                case CAMERA_MODE.THIRD_PERSON:
                    Camera.position = Target.position - GlobalOffset;
                    break;

            }
        }
     
        public void UpdateCameraMode(CAMERA_MODE mode) {
            CurrentMode = mode;
            switch (CurrentMode) {
                case CAMERA_MODE.FREE:
                    SetThirdPersonView();
                    break;
                case CAMERA_MODE.FIRST_PERSON:
                    SetFirstpersonView();
                    break;
                case CAMERA_MODE.THIRD_PERSON:
                    SetThirdPersonView();
                    GlobalOffset = Target.position - Camera.position;
                    break;
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

        private void SetThirdPersonView() {
            if (Target != null) {
                Camera.position = Target.position;
                Camera.rotation = Target.rotation;
                Camera.position += Camera.up * 1.05f;
                Camera.position += Camera.forward * -1.3f;
                Camera.position += Camera.right * 0.3f;
                Camera.RotateAround(Camera.position, Camera.right, 15f);
                GlobalOffset = Target.position - Camera.position;
            }
        }

        private void SetFirstpersonView() {
            Camera.position = Target.position;
            Camera.rotation = Target.rotation;
            Camera.position += Target.forward * 0.1f;
            Camera.position += Target.up * 0.5f;
            GlobalOffset = Target.position - Camera.position;
        }
    }

}
