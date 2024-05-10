//
//  ViewController.swift
//  ARDemo
//
//  
//

import UIKit
import SceneKit
import ARKit

class ViewController: UIViewController,ARSCNViewDelegate{

    @IBOutlet var sceneView: ARSCNView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // Set the view's delegate
        sceneView.delegate = self
        
        // Show statistics such as fps and timing information
        sceneView.showsStatistics = true
        sceneView.autoenablesDefaultLighting = true
//        // Create a new scene
        InfoLabel.isHidden = true
//        let scene = SCNScene(named: "art.scnassets/ship.scn")!
//        
//        // Set the scene to the view
//        sceneView.scene = scene
        // 创建一个新的AR会话
                let configuration = ARWorldTrackingConfiguration()
                sceneView.session.run(configuration)
        // 创建按钮并添加到视图中
//                createButtons()
    }
    @IBOutlet weak var InfoLabel: UILabel!
    @IBOutlet weak var GreenBtn: UIButton!
    @IBOutlet weak var PollutedBtn: UIButton!
    @IBAction func greenModePressed(_ sender: UIButton) {
        loadModel(named: "greenTurtle.usdz")
        sendModeRequest(mode: "Green")
        GreenBtn.isEnabled = false
        PollutedBtn.isEnabled = true
        InfoLabel.isHidden = true
    }

    @IBAction func pollutedModePressed(_ sender: UIButton) {
        loadModel(named: "t1.usdz")
        sendModeRequest(mode: "Polluted")
        GreenBtn.isEnabled = true
        PollutedBtn.isEnabled = false
        InfoLabel.isHidden = false
    }

    //加载模型
    func loadModel(named name: String) {
           sceneView.scene.rootNode.enumerateChildNodes { (node, _) in
            node.removeFromParentNode()
        }

        let modelNode = SCNNode()
        if let modelScene = SCNScene(named: name) {
            for child in modelScene.rootNode.childNodes {
                modelNode.addChildNode(child)
            }
        }
        modelNode.name = "modelName"
        modelNode.position = SCNVector3(x: 0, y: 0, z: -1)
        modelNode.scale = SCNVector3(x: 0.8, y: 0.8, z: 0.8)
        sceneView.scene.rootNode.addChildNode(modelNode)
//        // 将模型放置在最近检测到的平面上
//            if let lastPlaneNode = sceneView.scene.rootNode.childNodes.last(where: { $0.geometry is SCNPlane }) {
//                modelNode.position = lastPlaneNode.position
//                sceneView.scene.rootNode.addChildNode(modelNode)
//            }
    }
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?) {
        guard let touch = touches.first else { return }
        let location = touch.location(in: sceneView)
        let previousLocation = touch.previousLocation(in: sceneView)

        // 计算触摸移动的距离
        let deltaX = Float(location.x - previousLocation.x)
        let deltaY = Float(location.y - previousLocation.y)

        // 根据触摸移动的距离旋转模型
        if let modelNode = sceneView.scene.rootNode.childNode(withName: "modelName", recursively: true) {
            let angleX = deltaY * 0.01 // 旋转角度，你可以调整这个值以改变旋转速度
            let angleY = deltaX * 0.01 // 旋转角度，你可以调整这个值以改变旋转速度

            modelNode.eulerAngles.x += angleX
            modelNode.eulerAngles.y += angleY
        }
    }

    //发送请求给Node-Red
    func sendModeRequest(mode: String) {
        guard let url = URL(string: "http://192.168.0.103:1880/appRequest") else { return }
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.addValue("application/json", forHTTPHeaderField: "Content-Type")
        
        let body: [String: Any] = ["mode": mode]
        request.httpBody = try? JSONSerialization.data(withJSONObject: body, options: [])
        
        let task = URLSession.shared.dataTask(with: request) { (data, response, error) in
            if let error = error {
                print("Error sending mode request: \(error)")
                return
            }
            
            // Handle the response if needed
            if let httpResponse = response as? HTTPURLResponse, httpResponse.statusCode == 200 {
                print("Mode \(mode) sent successfully")
            } else {
                print("Failed to send mode \(mode)")
            }
        }
        task.resume()
    }
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        
        // Create a session configuration
        let configuration = ARWorldTrackingConfiguration()
        configuration.planeDetection = [.horizontal] // 开启水平平面检测

        // Run the view's session
        sceneView.session.run(configuration)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        
        // Pause the view's session
        sceneView.session.pause()
    }

    // MARK: - ARSCNViewDelegate
    
/*
    // Override to create and configure nodes for anchors added to the view's session.
    func renderer(_ renderer: SCNSceneRenderer, nodeFor anchor: ARAnchor) -> SCNNode? {
        let node = SCNNode()
     
        return node
    }
*/
    
    func session(_ session: ARSession, didFailWithError error: Error) {
        // Present an error message to the user
        
    }
    
    func sessionWasInterrupted(_ session: ARSession) {
        // Inform the user that the session has been interrupted, for example, by presenting an overlay
        
    }
    
    func sessionInterruptionEnded(_ session: ARSession) {
        // Reset tracking and/or remove existing anchors if consistent tracking is required
        
    }
}
//extension ViewController: ARSCNViewDelegate {
//    func renderer(_ renderer: SCNSceneRenderer, didAdd node: SCNNode, for anchor: ARAnchor) {
//        guard let planeAnchor = anchor as? ARPlaneAnchor else { return }
//
//        // 创建一个平面几何体来可视化检测到的平面
//        let plane = SCNPlane(width: CGFloat(planeAnchor.extent.x), height: CGFloat(planeAnchor.extent.z))
//        let planeNode = SCNNode(geometry: plane)
//        planeNode.position = SCNVector3(planeAnchor.center.x, 0, planeAnchor.center.z)
//        planeNode.eulerAngles.x = -.pi / 2
//        planeNode.opacity = 0.25 // 使平面半透明以便观察
//
//        // 将平面节点添加到场景中
//        node.addChildNode(planeNode)
//
//        // 如果已经加载了模型，更新其位置
//        if let modelNode = sceneView.scene.rootNode.childNode(withName: "modelName", recursively: true) {
//            modelNode.position = SCNVector3(planeAnchor.center.x, 0, planeAnchor.center.z)
//        }
//    }
//}

