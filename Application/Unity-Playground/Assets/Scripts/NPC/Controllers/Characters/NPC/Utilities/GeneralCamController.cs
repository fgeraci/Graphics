using UnityEngine;
using System.Collections;

public class GeneralCamController : MonoBehaviour {

    Transform gTransform;
    public bool ControlCamera;
    public float Speed = 1;

    private float gMouseX;
    private float gMouseY;
    public float CameraRotationSpeed = 20f;

	// Use this for initialization
	void Start () {
        gTransform = gameObject.transform;
	}
	
    void FixedUpdate() {
        if (Input.GetKey(KeyCode.F1)) {
            ControlCamera = !ControlCamera;
            Debug.Log("Camera Controller: " + (ControlCamera ? "Enabled" : "Disabled"));
        }
    }

	// Update is called once per frame
	void LateUpdate () {
	    if(ControlCamera) {
            float speedModifier = Input.GetKey(KeyCode.LeftShift) ? Speed * 2f : Speed;

            HandleCameraRotation();

            if(Input.GetKey(KeyCode.W)) {
                transform.position += transform.forward * (Time.deltaTime * speedModifier);
            } else if(Input.GetKey(KeyCode.S)) {
                transform.position -= transform.forward * (Time.deltaTime * speedModifier);
            }

            if(Input.GetKey(KeyCode.A)) {
                transform.position -= transform.right * (Time.deltaTime * speedModifier);
            } else if (Input.GetKey(KeyCode.D)) {
                transform.position += transform.right * (Time.deltaTime * speedModifier);
            }

            gMouseX = Input.mousePosition.x;
            gMouseY = Input.mousePosition.y;
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
}
