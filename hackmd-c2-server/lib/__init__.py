import json
from dataclasses import dataclass, asdict


@dataclass
class Target:
    name: str
    note_id: str = ""
    real_time_note_id: str = ""
    
    def to_dict(self) -> dict:
        return asdict(self)
    
    @classmethod
    def from_dict(cls, data: dict):
        return cls(**data)

STATUS_READY = 'ready'
STATUS_EXECUTING = 'executing'

@dataclass
class HackmdData:
    status: str
    request: str
    response: str
    save_path: str

    def to_dict(self) -> dict:
        return asdict(self)
    
    @classmethod
    def from_dict(cls, data: dict):
        return cls(**data)

    @classmethod
    def pack(cls, status: str, request: str, response: str, save_path: str) -> str:
        packed = {
            "status": status,
            "request": request,
            "response": response,
            "save_path": save_path
        }
        return json.dumps(packed)

    @classmethod
    def unpack(cls, packed_str: str):
        try: 
            data = json.loads(packed_str)
        except json.JSONDecodeError:
            data = {
                "status": STATUS_READY,
                "request": "",
                "response": "",
                "save_path": ""
            }
        return cls.from_dict(data)

INIT_CONTENT = HackmdData.pack(
    status=STATUS_READY,
    request="",
    response="",
    save_path=""
)

INIT_REAL_TIME_CONTENT = ""