//
//  LoadModelFile.swift
//  ARDemo
//
//  Created by 任圈圈 on 2024/3/4.
//

import UIKit
import SceneKit

class LoadModelFile: SCNNode {
    override init(){
        super.init()
        // 加载dae文件
//        guard  let url = Bundle.main.url(forResource: "baby_groot", withExtension: "dae") else {
//            fatalError("baby_groot.dae not exit.")
//        }
        // 加载obj文件
        guard  let url = Bundle.main.url(forResource: "butterfly", withExtension: "obj") else {
            fatalError("butterfly.obj not exit.")
        }
         //加载scn文件
        guard  let url = Bundle.main.url(forResource: "butterfly", withExtension: "scn") else {
            fatalError("bbutterfly.scn not exit.")
        }
        
        guard let customNode = SCNReferenceNode(url: url) else {
            fatalError("load butterfly error.")
        }
        customNode.load()
        self.addChildNode(customNode)
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}
