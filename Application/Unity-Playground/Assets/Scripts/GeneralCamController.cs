using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;

public class GeneralCamController : MonoBehaviour, INPCMessage {
    
    public float Speed = 1.0f;
    private float gMouseX;
    private float gMouseY;
    public float CameraRotationSpeed = 20f;

    public Transform Target;
    Transform MainCamera;
    Vector3 GlobalOffset;
    public CAMERA_MODE CurrentMode;

    List<INPCMessage> RegisteredListeners = new List<INPCMessage>();

    public enum CAMERA_MODE {
        FREE,
        THIRD_PERSON,
        FIRST_PERSON
    }

    // Use this for initialization
    void Awake () {
        MainCamera = Camera.main.transform;
        if (Target != null) {
            SetThirdPersonView();
            CurrentMode = CAMERA_MODE.THIRD_PERSON;
        } else {
            CurrentMode = CAMERA_MODE.FREE;
        }
    }
	
    void FixedUpdate() {
        if (Input.GetKey(KeyCode.F1)) {
            CurrentMode = CAMERA_MODE.FREE;
        } else if (Input.GetKey(KeyCode.F2)) {
            SetThirdPersonView();
            CurrentMode = CAMERA_MODE.THIRD_PERSON;
        } else if (Input.GetKey(KeyCode.F3)) {
            SetFirstpersonView();
            CurrentMode = CAMERA_MODE.FIRST_PERSON;
        }
    }
    
	void LateUpdate () {
        switch (CurrentMode) {
            case CAMERA_MODE.FREE:
                HandleFreeCamera();
                break;
            case CAMERA_MODE.FIRST_PERSON:
            case CAMERA_MODE.THIRD_PERSON:
                MainCamera.position = Target.position - GlobalOffset;
                break;
                
        }
        ServeMessage();
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
            MainCamera.position = Target.position;
            MainCamera.rotation = Target.rotation;
            MainCamera.position += MainCamera.up * 1.05f;
            MainCamera.position += MainCamera.forward * -1.3f;
            MainCamera.position += MainCamera.right * 0.3f;
            MainCamera.RotateAround(MainCamera.position, MainCamera.right, 15f);
            GlobalOffset = Target.position - MainCamera.position;
        }
    }

    private void SetFirstpersonView() {
        MainCamera.position = Target.position;
        MainCamera.rotation = Target.rotation;
        MainCamera.position += Target.forward * 0.1f;
        MainCamera.position += Target.up * 0.5f;
        GlobalOffset = Target.position - MainCamera.position;
    }

    #region INPCMessage
    public void UpdateListener(INPCMessage message) {
        throw new NotImplementedException();
    }

    public void RegisterListener(INPCMessage listener) {
        RegisteredListeners.Add(listener);
    }

    public void ServeMessage() {
        foreach (INPCMessage m in RegisteredListeners) {
            m.UpdateListener(this);
        }
    }
    #endregion
}
