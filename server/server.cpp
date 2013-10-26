#include "PassiveSocket.h"
#include <iostream>
#include <list>
#include "OpenThreads/Thread"
#include <OpenThreads/ReadWriteMutex>
#include "osg/Group"

struct ObjectState 
{
  ObjectState() {}
  ObjectState(unsigned int id, osg::Vec3d pos, double angle)
    : _id(id)
    , _pos(pos)
    , _angle(angle)
  {}

  unsigned int _id;
  osg::Vec3d _pos;
  double _angle;
};

class ServerData
{
public:
  ServerData(){}

  ObjectState* GetObjectState(unsigned int &size)
  {
    ObjectState* state = new ObjectState[_tankPositions.size()];
    std::map<unsigned int, std::pair<osg::Vec3d, double>>::iterator it = _tankPositions.begin();
    size = 0;

    {
      OpenThreads::ScopedReadLock lock(_rwMutex);
      for(; it != _tankPositions.end(); it++)
      {
        state[size]._id = it->first;
        state[size]._pos = it->second.first;
        state[size]._angle = it->second.second;
        size++;
      }
    }

    return state;
  }

  void SetPosition(unsigned int id, osg::Vec3d pos, double angle)
  {
    OpenThreads::ScopedWriteLock lock(_rwMutex);
    _tankPositions[id] = std::make_pair(pos, angle);
  }

protected:
  std::map<unsigned int, std::pair<osg::Vec3d, double>> _tankPositions;
  OpenThreads::ReadWriteMutex _rwMutex;
};

class ClientThread : public OpenThreads::Thread
{
public:

  ClientThread(CActiveSocket* client, unsigned int clientID, ServerData* srvData)
    : _client(client)
    , _clientID(clientID)
    , _srvData(srvData)
  {
    if (!isRunning()) start();
  }

  virtual void quit(bool waitForThreadToExit = true)
  {
    _done = true;
    if(isRunning() && waitForThreadToExit)
      join();
  }

  ~ClientThread() { quit(); }

private:
  virtual void run()
  {
    _done = false;

    _client->Send((const uint8 *)&_clientID, sizeof(unsigned int));

    while (!_done)
    {
      microSleep(100);

      if (_client->Receive(sizeof(ObjectState)))
      {
        ObjectState* tankState = (ObjectState*)_client->GetData();
        _srvData->SetPosition(_clientID, tankState->_pos, tankState->_angle);

        //printf("%f : %f\n", position->x(), position->y());

        unsigned int sizeSS = 0;
        ObjectState* ss = _srvData->GetObjectState(sizeSS);
        _client->Send((const uint8 *)&sizeSS, sizeof(unsigned int));
        _client->Send((const uint8 *)ss, sizeof(ObjectState) * sizeSS);
      }
    }

    _client->Close();
    delete _client;
  }

  bool _done;

  CActiveSocket* _client;
  unsigned int _clientID;
  ServerData* _srvData;
};

int main(int argc, char **argv)
{
  CPassiveSocket socket;
  CActiveSocket *pClient = NULL;

  std::list<ClientThread*> clients;
  ServerData* srvData = new ServerData();

  socket.Initialize();
  socket.Listen((const uint8 *)"127.0.0.1", 6789);

  unsigned int clientID = 0;

  while (true)
  {
    if ((pClient = socket.Accept()) != NULL)
    {
      clients.push_back(new ClientThread(pClient, clientID, srvData));
      clientID++;
    }
  }

  while(!clients.empty())
  {
    ClientThread* tmp = clients.front();
    clients.pop_front();
    delete tmp;
  }

  socket.Close();

  return 1;
}