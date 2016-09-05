using UnityEngine;
using UnityEngine.UI;
using System.Collections;

/// <summary>
/// Display Information on the screen for:
///     Camera
/// </summary>

public class UIController : MonoBehaviour {

    public Text CameraMode;
    GeneralCamController CamController;
    public bool Debug = true;

	// Use this for initialization
	void Start () {
	    CamController = Camera.main.GetComponent<GeneralCamController>();
    }
	
	// Update is called once per frame
	void Update () {
        if(Debug) {
            CameraInfo();
        }
	}

    /// <summary>
    /// Displays Camera Data
    /// </summary>
    void CameraInfo() {
        CameraMode.text = "Camera Mode: ";
        switch (CamController.CurrentMode) {
            case GeneralCamController.CAMERA_MODE.FIRST_PERSON:
                CameraMode.text += "First Person";
                break;
            case GeneralCamController.CAMERA_MODE.THIRD_PERSON:
                CameraMode.text += "Third Person";
                break;
            case GeneralCamController.CAMERA_MODE.FREE:
                CameraMode.text += "Free Flight";
                break;
        }
    }
}
