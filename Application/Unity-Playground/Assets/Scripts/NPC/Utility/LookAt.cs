using UnityEngine;
using System.Collections;

public class LookAt : MonoBehaviour {

    Vector3 follow;
    Vector3 curH,curV;

	// Use this for initialization
	void Start () {
        follow = Input.mousePosition;
	}
	
	// Update is called once per frame
	void Update () {
        LookAtTarget();
    }

    // do not use
    private void LookAtTargetVectors() {
        Vector3 mousePosition = Camera.main.ScreenPointToRay(Input.mousePosition).direction + transform.position;
        Vector3 target = Vector3.Normalize(mousePosition - transform.position);
        // Debug.DrawRay(transform.position, target * 3f, Color.red);

        Vector3 horizontalReflection = Vector3.Normalize(target - Vector3.Project(target, Vector3.up));
        Vector3 verticalReflection = Vector3.Normalize(target - Vector3.Project(target, Vector3.right));
        Debug.DrawRay(transform.position, horizontalReflection * 5f, Color.blue);
        Debug.DrawRay(transform.position, verticalReflection * 5f, Color.green);
        float hAngle = Vector3.Angle(curH, horizontalReflection);
        float vAngle = Vector3.Angle(curV, verticalReflection);
        Debug.DrawRay(transform.position, Vector3.up, Color.yellow);
        Debug.DrawRay(transform.position, Vector3.right, Color.yellow);
        transform.Rotate(Vector3.right, vAngle);
        Debug.Log(mousePosition);
        follow = mousePosition;
        curH = horizontalReflection;
        curV = verticalReflection;
    }

    private void LookAtTarget() {
        Vector3 mousePosition = Camera.main.ScreenPointToRay(Input.mousePosition).direction + transform.position * 5f;
        if (mousePosition != follow) {
            Vector3 lookAtDirection = Vector3.Normalize(mousePosition - transform.position);
            Vector3 pitchAxis;
            Vector3 yawAxis;

            // orthogonal projection of pitch axis
            pitchAxis = Vector3.Normalize(Vector3.up - (Vector3.Project(lookAtDirection, Vector3.up)));

            // orthogonal to forward and up
            yawAxis = Vector3.Cross(pitchAxis, lookAtDirection);

            // Change the coordinates
            transform.right = new Vector3(yawAxis.x, yawAxis.y, yawAxis.z);
            transform.up = new Vector3(pitchAxis.x, pitchAxis.y, pitchAxis.z);
            transform.forward = new Vector3(lookAtDirection.x, lookAtDirection.y, lookAtDirection.z);

            Debug.Log("Up: " + pitchAxis + " Right: " + yawAxis + " Forward: " + lookAtDirection);

            // obtain the direction vector for the target
            Debug.DrawRay(transform.position, lookAtDirection * 2f, Color.red);
            Debug.DrawRay(transform.position, pitchAxis * 2f, Color.yellow);
            Debug.DrawRay(transform.position, yawAxis * 2f, Color.green);
        }
    }
}
