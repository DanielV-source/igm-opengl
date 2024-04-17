//
// Created by daniel on 17/04/24.
//


#include <iostream>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include <osg/PositionAttitudeTransform>



// Define a rotation callback class
class SpinCallback : public osg::NodeCallback
{
public:
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
    {
        osg::PositionAttitudeTransform* pat = dynamic_cast<osg::PositionAttitudeTransform*>(node);
        if (pat)
        {
            double angle = osg::inDegrees(nv->getFrameStamp()->getSimulationTime() * 40.0); // 40 degrees per second
            pat->setAttitude(osg::Quat(angle, osg::Vec3(0, 1, 1))); // Rotate about the Z-axis
        }
        traverse(node, nv);
    }
};



int main(int argc, char* argv[])
{
    // Check command-line parameters
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <model file>\n";
        exit(1);
    }

    // Load the model
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile(argv[1]);
    if (!loadedModel)
    {
        std::cerr << "Problem opening '" << argv[1] << "'\n";
        exit(1);
    }

    osg::ref_ptr<osg::Group> root (new osg::Group());
    const double translation = 2.2 * loadedModel->getBound().radius();

    // Create the first spinning object
    osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform;
    pat->addChild(loadedModel);
    pat->setUpdateCallback(new SpinCallback);

    // Create the second spinning object
    osg::ref_ptr<osg::PositionAttitudeTransform> ppat1 = new osg::PositionAttitudeTransform;
    ppat1->addChild(pat);
    ppat1->setPosition(osg::Vec3(0.0, 0.0, 0.0));

    osg::ref_ptr<osg::PositionAttitudeTransform> ppat2 = new osg::PositionAttitudeTransform;
    ppat2->addChild(pat);
    ppat2->setPosition(osg::Vec3(translation, 0.0, 0.0));

    // Add both spinning objects to the root group
    root->addChild(ppat1);
    root->addChild(ppat2);

    // Create a viewer, use it to view the model
    osgViewer::Viewer viewer;
    viewer.setSceneData(root);

    // Set a basic trackball manipulator to enable user interaction with the view
    viewer.setCameraManipulator(new osgGA::TrackballManipulator);

    // Enter rendering loop
    viewer.run();
}