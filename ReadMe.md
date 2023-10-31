## QT-ClothesCAD


#### 1 Function Selection and Main Interface Tools

##### 1.1 Five Continuous Functions

---

##### 1.2 Requirement Description

| Function        | Requirements                                                                                                            |
|-----------------|--------------------------------------------------------------------------------------------------------------------------|
| Ellipse         | 1. Draw an ellipse using mouse drag<br>2. Set ellipse’s length and width through a dialog box                            |
| Text            | 1. Add text<br>2. Set angle<br>3. Set width<br>4. Adjust text<br>5. Move                                                 |
| Line Adjustment | 1. Various adjustments related to curves<br>2. Endpoint adjustment<br>3. Synchronous changes of a group of lines         |
| Eraser          | 1. Delete a single construction<br>2. Delete a group of constructions<br>3. Delete the entire drawing                    |
| Modify          | 1. Adjust line<br>2. Add/remove points on line<br>3. Offset points on line<br>4. Adjust folded line curves              |

---

##### 1.3 Main Interface Toolbar

---

##### 1.4 Notes

1. The implementation of each function may involve using 1-4 types of tools from the toolbar, which I will introduce step by step below.
2. For the operation of each function, pay attention to: click, double click, left button, right button, and keyboard input.
3. All functions listed in the table above have been implemented (the implementation method may not be completely the same as in the video), and the implementation results will be presented and technically explained below, function by function.
4. Most of the other toolbar tools that are not required for the functions can be used normally, but ‘new’ in the file cannot be used, and ‘help’ is just for decoration.
5. To realize synchronous adjustment of line adjustment, the rectangle tool is not the normal CAD rectangle function, but simulates two pieces of clothing templates.
6. To realize the modify function, the line tool is not the normal CAD line function, but a folded line where dividing points can be inserted.
7. In the interface design, we referred to the CAD toolbar setting on the left and rendered the skin with qss (test.qss file), making the appearance comfortable. The button’s layout is flow layout, and the interface has a minimum width set, so that one page can accommodate more screenshots. However, this makes it less aesthetic. Since the button images are in png format which is transparent, we chose white buttons with a black background.
8. In the instructions below, I will first provide a technical explanation of some points to note for that function, which will be concise. After all the functions have been explained, there will be an additional technical explanation, which will help save your time during review.

   

#### 2 Ellipse

##### 2.1 Implementation Result

1. Left-clicking the ellipse button will display the following interface

2. Clicking and dragging the mouse with the left button will form an ellipse

3. Double-clicking the right mouse button brings up a dialog box for setting the ellipse's length and width

4. Upon pressing OK, the ellipse will change to the ellipse corresponding to the values we set

##### 2.2 Technical Explanation

1. Two rectangle classes are defined in `rectangle.h`, one of which controls vector features such as length, width, position, angle, etc., while the other controls scalar features such as color. All vector features are denoted with VE.

2. In the class controlling vector features, control methods corresponding to the ellipse are set for different mouse and keyboard events.

3. It is important to note that the length and width settings need to receive custom parameters from the user through the `ellchange.ui` interface. For this reason, `ellchange.h` and `ellchange.cpp` are set up, as well as an input range.


   ```c++
   ellchange::ellchange(QWidget *parent) :
       QDialog(parent),
       ui(new Ui::ellchange)
   {
       ui->setupUi(this);
       ui->doublehigh->setMaximum(100);
       ui->doublehigh->setMinimum(15);
       ui->doublewide->setMinimum(150);
       ui->doublewide->setMaximum(1000);
   }
   ```

4. The code for generating the ellipse by dragging the mouse is as follows:

   ````c++
   if (m_leftMouseButtonPressed) {
       setCursor(Qt::ClosedHandCursor);
       auto dx = event->scenePos().x() - m_previousPosition.x();
       auto dy = event->scenePos().y() - m_previousPosition.y();
       moveBy(dx,dy);
       setPreviousPosition(event->scenePos());
       emit signalMove(this, dx, dy);
   }
   ````

   

### 3 Text

##### 3.1 Implementation Result

1. Left-click the text button, then left-click on the blank screen, and the following dialog box will appear. Set the parameters according to your preferences.

2. Left-click OK, and the screen will display the following dialog box.

3. Click the selection tool (arrow) in the toolbar, and then double-click inside the text input area indicated in the box to change the text.

4. If you need to move the text, also click the selection tool in the toolbar, select the area between the outer and inner frames, and then you can move it.

##### 3.2 Technical Explanation

1. Firstly, considering that modifying text requires selecting the text, but the move tool requires selecting the entire object, if only one frame is set, it might cause confusion. Therefore, a smaller text box and a larger rectangular frame are set, and their changes are synchronized. Text descriptions are added inside the text box.


```c++
// Text box prompt
text_item->setPlainText(trUtf8("Please enter text here"));
// The two frames have a margin of 3*adjust
rect_item->setRect(500, 500,
                   text_rect.width() + 3*adjust,
                   text_rect.height() + 3*adjust);
// Synchronization
void TextItem::reset_text_rect(void)
{
    text_rect = text_item->boundingRect();
}


2. The mechanism for receiving custom parameters from the user through the text.ui interface is similar to that of the ellipse class, and will not be repeated here.

3. It’s important to note that I have set the text function to be of the same category as rectangle, ellipse, and other tools. Initially, I implemented the text mechanism separately. However, this approach has a potential issue: it may not switch smoothly between tools. For example, after drawing geometric shapes, you may not be able to respond normally when switching to the text tool.

4. Here, I will present the mouse events related to text.

   ```c++
   //void VEWorkplace::mousePressEvent(QGraphicsSceneMouseEvent *event)
   case TextType: {
           if (m_leftMouseButtonPressed && !(event->button() & Qt::RightButton) && !(event->button() & Qt::MiddleButton)) {
               deselectItems();
               TextItem *txtItem;
               try {
                   txtItem = new TextItem();
               } catch(...) {
                   break;
               }
               currentItem = txtItem;
               addItem(currentItem);
               connect(txtItem, &TextItem::clicked, this, &VEWorkplace::signalSelectItem);
               connect(txtItem, &TextItem::signalMove, this, &VEWorkplace::slotMove);
               emit signalNewSelectItem(txtItem);
           }
           break;
       }
   
   ```


It should be noted that there is a rectangular object outside the text. For functionalities that the text object cannot respond to, the rectangular object can be used to achieve them, such as moving:

   ```c++
   //void VEWorkplace::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
   case RectangleType: {
           if (m_leftMouseButtonPressed) {
               auto dx = event->scenePos().x() - m_previousPosition.x();
               auto dy = event->scenePos().y() - m_previousPosition.y();
               VERectangle * rectangle = 
                   qgraphicsitem_cast<VERectangle *>(currentItem);
               rectangle->setRect(
                  (dx > 0) ? m_previousPosition.x() : event->scenePos().x(),
                  (dy > 0) ? m_previousPosition.y() : event->scenePos().y(),
                  qAbs(dx), qAbs(dy));
           }
           break;
       }
   ```

   

### 4 Line Adjustment

##### 4.1 Implementation Result

###### 4.1.1 Adjustment of Individual Lines or Points

1. Click the curve tool, and a line will appear on the interface.

2. Click the selection tool (arrow) in the toolbar, then click on the line in the interface. To our surprise, there are control points on it, and both blue and black lines appear. The blue line is for adjustment, and the black line is the actual curve.

3. Curve Adjustment: Drag the endpoints of the blue control line with the left mouse button.

4. Point Adjustment: Drag the control points on the black curve with the left mouse button.

###### 4.1.2 Implementation of Synchronized Adjustment

1. Double-click the rectangle tool with the left mouse button, then press and hold the left mouse button to draw a rectangle on the screen. Another rectangle will automatically appear, intended to be adjusted synchronously with the first one. Let's name the rectangle we drew as Rectangle 1, and the automatically generated one as Rectangle 2.

2. Click the selection tool, then slightly drag the top edge of Rectangle 1 (width adjustments won’t work, as the code is only written for height synchronization). After that, the top edge of Rectangle 2 will automatically align with Rectangle 1. Note that this feature can only respond once.

3. Afterwards, adjust the top edge of either Rectangle 1 or Rectangle 2 at will, and both rectangles will be adjusted synchronously.

##### 4.2 Technical Explanation

1. This is a relatively difficult feature. First of all, the curve control points here are preset by me, so when you click the button, a fixed line will appear.


   ```c++
   void MainWindow::on_toolButton_bezier_clicked()
   {
       VEComplexShapeItem * item4 = new VEComplexShapeItem(workplaceScene);
       item4->addPoint(QPointF(400,400),VEComplexShapeItem::SEGEMENT_LINE);
           //Omit 5 lines of redundant operations.
       item4->addPoint(QPointF(700,700),VEComplexShapeItem::SEGEMENT_LINE);
   }
   ```

2. To enable the control line and curve to be identified and separated, parameters are set for the two objects separately. The parameters here are also built-in.

   ```c++
       for(int i=0 ; i < mHandles.size() ; i++) {
           if(i == 0) {
               path.moveTo(mHandles.at(0)->pos());
           } else {
               QLineF line1(path.currentPosition(),mHandles.at(i)->pos());
               QLineF line2(mHandles.at(i+1)->pos(),mHandles.at(i+2)->pos());
               path.cubicTo(mHandles.at(i)->pos(),
                            mHandles.at(i+1)->pos(),mHandles.at(i+2)->pos());
               if(this->isSelected()) {
                   //控制线
                   pen.setWidth(2);
                   pen.setColor(QColor(128, 128, 255));
                   painter->setPen(pen);
                   painter->drawLine(line1);
                   painter->drawLine(line2);
                   pen.setWidth(1);
                   pen.setColor(QColor(0, 0, 0));
                   painter->setPen(pen);
               }
               i = i+2;
           }
       }
       painter->drawPath(path);
   ```

3. The type of curve selected here is a Bézier curve. If you are interested, you can also try natural spline curves, clamped spline curves, or iterate using Newton's method in numerical analysis. There are ready-made formulas for these curves. The adjustment method referred to here is borrowed from the pen tool in Adobe Illustrator.

4. The principle of synchronous adjustment goes like this: suppose there is a control block and a follow block. Directly making the follow block respond to the control block can be somewhat difficult to implement. Therefore, a third intermediary block is introduced in the code to connect the two.

This is a rather challenging point, mainly relying on the move.h header file. I believe the header file below can help you understand better.

   ```c++
   template <typename T>
   class signal_group;
   
   template <typename T>
   class signalable {
           signal_group<T>* group;
           friend class signal_group<T>;
   public:
           signalable(): group(nullptr) {}
           virtual void handleSignal(int type, void* value) = 0;
           void sendSignal(int type, void* value) {
                   group->sendSignal(type, value);
           }
   };
   
   template <typename T>
   class signal_group: private std::vector<T*> {
   public:
           void sendSignal(int type, void* value) {
                   for(auto i : *this) {
                           i->handleSignal(type, value);
                   }
           }
           T* operator[](int id) {
                   return std::vector<T*>::operator[](id);
           }
           T* add_item(T* val) {
                   if(val->signalable<T>::group) throw;
                   val->signalable<T>::group = this;
                   std::vector<T*>::push_back(val);
                   return val;
           }
   };
   ```

   
### 5 Eraser

##### 5.1 Implementation Results

1. Deletion of a Single Object: First, we draw many lines on the interface, click the select function with the left mouse button, and then left-click to select the line we want to delete. Press the “delete” key on the keyboard to delete.

2. Select and Delete Multiple Objects at the Same Time: After selecting an object, you can continue to select objects while holding down the “CTRL” key. To delete, press the “delete” key on the keyboard.

3. One-click Clear: There is a large cross in the toolbar, press it to clear everything.

##### 5.1.2 Technical Description

1. In the response mechanism when the mouse is pressed down, the corresponding selection class feedback is set.


   ```
   case SelectionType: {
           if (m_leftMouseButtonPressed && !(event->button() & Qt::RightButton) 
                  && !(event->button() & Qt::MiddleButton)) {
               deselectItems();
               VESelectionRect *selection = new VESelectionRect();
               currentItem = selection;
               addItem(currentItem);
           }
           break;
       }
   ```

2. Here is a particular case: among these five functions, this is one that utilizes keyboard control for completion. The keyboard control response mechanism is as follows. I will use 'delete' as an example and also explain how to perform a deletion.


   ```c++
   //delete的键盘响应
   case Qt::Key_Delete: {
           foreach (QGraphicsItem *item, selectedItems()) {
               removeItem(item);
               delete item;
           }
           deselectItems();
           break;
   }
   //delete的实现
   foreach (QGraphicsItem *item, selectedItems()) {
           item->setSelected(false);
       }
   selectedItems().clear();
   ```

   

### 6 Adjustment

##### 6.1.1 Implementation Results

1. Adjusting Lines: First, draw a line using the line tool. With the selection tool, you can freely adjust the angle, length, and position. Before drawing, you can set the line width and color.

2. Adding Points to a Line: Click the selection tool with the left mouse button, then double-click on the line with the left mouse button to add a point. In the image below, to show that a point has been added, I have moved the position of this point appropriately (it can be dragged directly, I will introduce relative displacement in the next step).

3. Offset of Points on the Line: Double-right-click on the point we added, and the following dialog box will appear.

4. Press OK, and the point will translate according to the set values, and the line will change accordingly.

5. Converting Curves to Polyline: In the curve tool, when the two control lines at both ends of a line segment combine into a straight line, the curve becomes a polyline.

##### 6.1.2 Technical Description

1. The technical challenge here is the control of the points. If we follow the parameter passing method used for the ellipse earlier and receive parameters directly calling $$moveBy(dx, dy)$$, it would result in an error. Additionally, updating the path is challenging. The difficulty lies in how to make the line recognize which point you have changed. It is necessary to record the point’s information when passing parameters.


   ```c++
   void DotSignal:: mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event){
       qreal dx = 0;
       qreal dy = 0;
       bool moved = false;
       {
           if(dot.exec() == QDialog::Accepted)
               {
                dx = dot.getleft()-dot.getright();
               dy = dot.getup()-dot.getdown();
               moved = true;
           }else{
           QGraphicsItem::mouseDoubleClickEvent(event);
           }
       }
       if(moved){
           auto p = ((VEPolyline*)parentItem())->path();
           p.setElementPositionAt(id, p.elementAt(id).x - dx, p.elementAt(id).y - dy);
           ((VEPolyline*)parentItem())->setPath(p);
           ((VEPolyline*)parentItem())->updateDots();
           moveBy(dx,dy);
       }
   }
   ```

2. Regarding adding and removing points on a line, the principle of adding points is relatively easy to understand. Assume there is an original line with two endpoints. Adding one point turns it into two lines, and this point becomes the starting point of one line and the endpoint of the other.

3. Regarding removing points, there is a function called: $$checkForDeletePoints()$$

4. Here, we need to pay attention to the update of points: $$updateDots()$$

5. Similar to the ellipse, the vector and scalar of the line are also divided into two classes, which will not be elaborated on here again.

6. About how to add points


   ```c++
       int size = settings.value("drawing/hanleSize",4).toInt();
       mHandles<<new Handle(point,size,Handle::
                 HANDLE_SHAPE_CIRCLE,Handle::HANDLE_TYPE_CTRL);
       this->recalculateRect();
   ```

   

### 7 Supplementary Technical Explanation

##### 7.1 Response Class

Entering event response from the workspace interface message:

```c++
QGraphicsView::enterEvent(event);
viewport()->setCursor(Qt::CrossCursor);

```

Mouse release event response in the workspace interface:

```
QGraphicsView::mouseReleaseEvent(event);
viewport()->setCursor(Qt::CrossCursor);
```

Mouse and keyboard response:

```
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
```

signals response：

```
    void previousPositionChanged();
    void currentActionChanged(int);
    void signalSelectItem(QGraphicsItem *item);
    void signalNewSelectItem(QGraphicsItem *item);
```

Bounce Response：

```
connect(ui->butLine, &QToolButton::clicked, [=](){workplaceScene-       >setCurrentAction(VEWorkplace::LineType);});
connect(workplaceScene, &VEWorkplace::signalNewSelectItem, this, &MainWindow::selectNewItem);
```

UI direction：

```
    ui->splitter->setStretchFactor(1,1);
    ui->rectangleSettings->setVisible(false);
```

##### 7.2 Event Class

Move：

```
foreach (QGraphicsItem *item, selectedItems()) {
        if(item != signalOwner) item->moveBy(dx,dy);
```

Delete：

```
foreach (QGraphicsItem *item, selectedItems()) {
        item->setSelected(false);
    }
    selectedItems().clear();
```

Update Position, Size, Angle:

```
        QPointF left(mRect.left(),mRect.top() + mRect.height()/2);
        QPointF right(mRect.right(),mRect.top() + mRect.height()/2);
        QPointF top(mRect.left()+mRect.width()/2,mRect.top());
        QPointF bottom(mRect.left()+mRect.width()/2,mRect.bottom());
        QPointF rotate(top.x(),top.y()-50);
```

```
        case Handle::HANDLE_TYPE_LEFT:
            this->mRect.setLeft(event->pos().x());
            mOrigin.setX((dx * mRect.width()) + mRect.center().x());
            break;
```

Color :

```
ui->color_2->setVisible(true);
        ui->labelColor_2->setVisible(true);
        ui->labelColor_1->setText(trUtf8("<font color='white'>颜色1</font>"));
        if(currentEllipse != nullptr){
            setGradient(currentEllipse);
```

Gain information：

```
    qreal angleToTarget = ::acos(lineToTarget.dx() / lineToTarget.length());
    if (lineToTarget.dy() < 0)
        angleToTarget = TwoPi - angleToTarget;
    angleToTarget = normalizeAngle((Pi - angleToTarget) + Pi / 2);
```

Give Setting：

```
void Handle::setType(HandleType type) {
    this->mType = type;
}
```

#### How to use

This repository contains two folders: 1. release 2. yzh3551.

In the 'release' folder, you will find the executable files for the release version. However, upon double-clicking to run the program, you might encounter a system error stating that "Qt5Svg.dll" could not be found. This error indicates that the environment required to run the executable is missing. To resolve this, you have two options:

Use the command line to provide the environment variable to the executable. The program requires DLL dynamic link libraries, so you should add the program's installation directory (e.g., X:\Qt5.0.2\bin) to your system's Path environment variable.

Locate the folder on your computer where Qt5Svg.dll is saved and copy the executable file into that folder. You should then be able to run the program.

I have provided my version of Qt5Svg.dll. If the application fails to start with the error 0xc000007b, you can try the following:

Click on the Start menu in the bottom left corner of your computer screen.
Enter "cmd.exe" in the search bar and press Enter to open the Command Prompt with administrative privileges.
In the Command Prompt, type "sfc /scannow" and press Enter. After the scan completes, the issue should be resolved.
The 'yzh3551' folder contains the source code for the program. You can also run the program directly from Qt5 by opening the "yzh3551.pro" file located in this folder. If you encounter any issues with compiling the program, you might need to set the build output path to match the directory structure on your computer. Alternatively, you can copy my program to the location on your computer where the build outputs are generated.

Please note that if you are using Visual Studio, the file extension for project files is .sln, whereas .pro is the file extension used by QT Creator. The 'icons' folder contains various icons used to enhance the appearance of the application, and these icons can be downloaded from the internet. The required files for the project include .h, .cpp, .ui, and .qss files, where .qss files are used for interface rendering.

You may need to rebuild all files before compiling and running the program. If your computer environment and QT environment are significantly different from mine, I have included screenshots of the entire required process in the report.
