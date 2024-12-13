void MainWindow::on_draw_clicked() {
    graph.clear();
    try{
        QString code = ui->code->toPlainText();
        QString tokens = ui->tokens->toPlainText();
        Node root = getNodes(code,tokens);
        std::map<Node, std::vector<std::pair<Node, bool>>> adjList = graph;

        QGraphicsScene *scene = new QGraphicsScene(this);
        ui->tree->setScene(scene);

        int offset = 50;
        int y = 100;
        int x = 150;

        std::map<int,int>currentForVLevel;
        QFont font("Arial", 12);


        std::function<void(Node,int)> dfs = [&](Node currentNode,int vLevel) {
            QGraphicsRectItem* nodeItemRect;
            QGraphicsEllipseItem* nodeItemEllipse;
            qDebug()<<QString::fromStdString(currentNode.name)<<" "<<QString::fromStdString(currentNode.value)<<'\n';
            int hLevel = currentForVLevel[vLevel] + 1;
            int parentX =  50 + (hLevel - 1) * x;
            currentForVLevel[vLevel] = currentForVLevel[vLevel] + 1;
            int parentY =  50 + (vLevel - 1) * y;


            if (currentNode.type == 1) {
                nodeItemRect = scene->addRect(parentX, parentY,100, 50, QPen(Qt::black), QBrush(Qt::white));
            } else {
                nodeItemEllipse = scene->addEllipse(parentX, parentY, 100, 50, QPen(Qt::black), QBrush(Qt::white));
            }

            QGraphicsTextItem *textItem = scene->addText(QString::fromStdString(currentNode.name));
            qreal xOffset = parentX + (100 - textItem->boundingRect().width()) / 2;
            qreal yOffset = parentY + (50 - textItem->boundingRect().height()) / 2;
            textItem->setPos(xOffset, yOffset - 10);
            textItem->setDefaultTextColor(Qt::black);

            if (currentNode.value != "") {
                QGraphicsTextItem *valueItem = scene->addText("(" + QString::fromStdString(currentNode.value) + ")");
                valueItem->setFont(font);
                valueItem->setDefaultTextColor(Qt::black);
                valueItem->setPos(parentX + (100 - valueItem->boundingRect().width()) / 2, parentY + 20);
            }

            for (const auto &p : adjList[currentNode]) {
                Node neighbor = p.first;
                bool isChild = p.second;
                if (isChild) {
                    currentForVLevel[vLevel+1] = std::max(currentForVLevel[vLevel] - 1,currentForVLevel[vLevel+1]);
                    int childY = 50 + vLevel * y;
                    int childX = 50 + (currentForVLevel[vLevel+1]) * x;
                    scene->addLine(parentX + 50, parentY + 50, childX + 50, childY, QPen(Qt::black));
                    dfs(neighbor, vLevel + 1);
                } else {
                    scene->addLine(parentX + 100, parentY + 25, parentX + 100 + 50, parentY + 25, QPen(Qt::black));
                    dfs(neighbor,vLevel);
                }
            }
        };


        dfs(root, 1);


        ui->tree->setRenderHint(QPainter::Antialiasing);
        ui->tree->setRenderHint(QPainter::SmoothPixmapTransform);
        ui->tree->setDragMode(QGraphicsView::ScrollHandDrag);
    }
    catch (const std::runtime_error &e) {
        showError(QString("An error occurred: %1").arg(e.what()), this);
    } catch (...) {
        showError(QString("An unknown error occurred."), this);
    }
}


void MainWindow::wheelEvent(QWheelEvent *event) {
    QRect r(ui->tree->rect().x() + 530, ui->tree->rect().y() + 40,  ui->tree->rect().width(),  ui->tree->rect().height());
    if (r.contains(event->position().toPoint())) {
        qreal factor = 1.1;
        qreal newScale = ui->tree->transform().m11();


        if (event->angleDelta().y() > 0) {
            if (newScale < 3.0) {
                ui->tree->scale(factor, factor);
            }
        }

        else {
            if (newScale > 0.3) {
                ui->tree->scale(1.0 / factor, 1.0 / factor);
            }
        }
        ui->tree->centerOn(event->position().toPoint());
    }
}

